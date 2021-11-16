#include <game/game_manager.h>

#include "utils/log.h"
#include <fmt/format.h>
#include <imgui.h>

#include "utils/conversion.h"

namespace game
{

    GameManager::GameManager() :
        transformManager_(entityManager_),
        rollbackManager_(*this, entityManager_),
        physicsManager_(entityManager_)

    {
        playerEntityMap_.fill(core::EntityManager::INVALID_ENTITY);
        
    }

    void GameManager::SpawnPlayer(PlayerNumber playerNumber, core::Vec2f position, core::degree_t rotation)
    {
        if (GetEntityFromPlayerNumber(playerNumber) != core::EntityManager::INVALID_ENTITY)
            return;
        core::LogDebug("[GameManager] Spawning new player");
        const auto entity = entityManager_.CreateEntity();
        playerEntityMap_[playerNumber] = entity;

        transformManager_.AddComponent(entity);
        transformManager_.SetPosition(entity, position);
        transformManager_.SetRotation(entity, rotation);
        rollbackManager_.SpawnPlayer(playerNumber, entity, position, core::degree_t(rotation));
    }

    core::Entity GameManager::SpawnBox(core::Vec2f position)
    { 
        core::LogDebug("SpawnBoxGameManager");
        const auto boxEntity = entityManager_.CreateEntity();
        transformManager_.AddComponent(boxEntity);
        transformManager_.SetPosition(boxEntity, position);
        rollbackManager_.SpawnBox(boxEntity, position);
        return boxEntity;
    }

    core::Entity GameManager::SpawnFlag(core::Vec2f position)
    {
        core::LogDebug("SpawnFlagGameManager");
        const auto flagEntity = entityManager_.CreateEntity();
        transformManager_.AddComponent(flagEntity);
        transformManager_.SetPosition(flagEntity, position);
        rollbackManager_.SpawnFlag(flagEntity, position);
        return flagEntity;
    }

    core::Entity GameManager::SpawnTrack(core::Vec2f position)
    {
        core::LogDebug("SpawnTrackGameManager");
        const auto trackEntity = entityManager_.CreateEntity();
        transformManager_.AddComponent(trackEntity);
        transformManager_.SetPosition(trackEntity, position);
        rollbackManager_.SpawnTrack(trackEntity, position);
        return trackEntity;
    }

    core::Entity GameManager::SpawnWall(core::Vec2f position)
    {
        core::LogDebug("SpawnWallGameManager");
        const auto wallEntity = entityManager_.CreateEntity();
        transformManager_.AddComponent(wallEntity);
        transformManager_.SetPosition(wallEntity, position);
        rollbackManager_.SpawnWall(wallEntity, position);
        return wallEntity;
    }

    core::Entity GameManager::SpawnGreatBox(core::Vec2f position)
    {
        core::LogDebug("SpawnGreatBoxGameManager");
        const auto greatBoxEntity = entityManager_.CreateEntity();
        transformManager_.AddComponent(greatBoxEntity);
        transformManager_.SetPosition(greatBoxEntity, position);
        rollbackManager_.SpawnGreatBox(greatBoxEntity, position);
        return greatBoxEntity;
    }

    void GameManager::SpawnLevel()
    {
        core::LogDebug("SpawnLevel");
        
        // Spawn the whole level taking the position of every single gameobject
        SpawnTrack(core::Vec2f(0, 20)); SpawnTrack(core::Vec2f(0, 40)); SpawnTrack(core::Vec2f(0, 60));
        SpawnTrack(core::Vec2f(0, 80)); SpawnTrack(core::Vec2f(0, 100));
        
        SpawnGreatBox(core::Vec2f(-2, -5)); SpawnGreatBox(core::Vec2f(2, -5));

        SpawnBox(core::Vec2f(-3, 3));SpawnBox(core::Vec2f(3, 3));SpawnBox(core::Vec2f(0, 8));
        SpawnBox(core::Vec2f(3, 11));SpawnBox(core::Vec2f(-3, 11));SpawnBox(core::Vec2f(1.5, 13));
        SpawnBox(core::Vec2f(-1.5, 13));SpawnBox(core::Vec2f(2, 16));SpawnBox(core::Vec2f(-2, 16));
        SpawnBox(core::Vec2f(0, 20));SpawnBox(core::Vec2f(-3, 26));SpawnBox(core::Vec2f(3, 26));
        SpawnBox(core::Vec2f(1, 28));SpawnBox(core::Vec2f(-1.5, 31));SpawnBox(core::Vec2f(3, 40));
        SpawnBox(core::Vec2f(2, 47));SpawnBox(core::Vec2f(1, 46));SpawnBox(core::Vec2f(-3, 50));
        SpawnBox(core::Vec2f(-1, 55));SpawnBox(core::Vec2f(2, 58));
        SpawnGreatBox(core::Vec2f(0, 65));
        SpawnBox(core::Vec2f(-2.5, 70)); SpawnBox(core::Vec2f(2.5, 70));
        SpawnGreatBox(core::Vec2f(1, 77));
        SpawnGreatBox(core::Vec2f(-1, 85));
        SpawnBox(core::Vec2f(3, 88));
        SpawnBox(core::Vec2f(1, 90));SpawnBox(core::Vec2f(-1, 92));SpawnBox(core::Vec2f(-2, 94));

        SpawnWall(core::Vec2f(4, 50)); SpawnWall(core::Vec2f(-4, 50));
        

        SpawnFlag(core::Vec2f(0, 100));SpawnFlag(core::Vec2f(-2, 100));SpawnFlag(core::Vec2f(2, 100));
        SpawnFlag(core::Vec2f(-1, 100));SpawnFlag(core::Vec2f(1, 100));SpawnFlag(core::Vec2f(3, 100));
        SpawnFlag(core::Vec2f(-3, 100));
    }

    core::Entity GameManager::GetEntityFromPlayerNumber(PlayerNumber playerNumber) const
    {
        return playerEntityMap_[playerNumber];
    }


    void GameManager::SetPlayerInput(PlayerNumber playerNumber, PlayerInput playerInput, std::uint32_t inputFrame)
    {
        if (playerNumber == INVALID_PLAYER)
            return;

        rollbackManager_.SetPlayerInput(playerNumber, playerInput, inputFrame);

    }
    void GameManager::Validate(Frame newValidateFrame)
    {
        if (rollbackManager_.GetCurrentFrame() < newValidateFrame)
        {
            rollbackManager_.StartNewFrame(newValidateFrame);
        }
        rollbackManager_.ValidateFrame(newValidateFrame);
    }


    PlayerNumber GameManager::CheckWinner() const
    {
        int firstPlayer = 0;
        PlayerNumber winner = INVALID_PLAYER;
        const auto& playerManager = rollbackManager_.GetPlayerCharacterManager();
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::PLAYER_CHARACTER)))
                continue;
            const auto& player = playerManager.GetComponent(entity);

            //use the player1win or player2win variable to determine which player won
            if (player.player1win >= 1)
            {
                firstPlayer++;
                winner = 0;
            }
            else if (player.player2win >= 1)
            {
                firstPlayer++;
                winner = 1;
            }

        }

        return firstPlayer == 1 ? winner : INVALID_PLAYER;
    }


    void GameManager::WinGame(PlayerNumber winner)
    {
        winner_ = winner;
    }

    ClientGameManager::ClientGameManager(PacketSenderInterface& packetSenderInterface) :
        GameManager(),
        spriteManager_(entityManager_, transformManager_),
        packetSenderInterface_(packetSenderInterface)
    {
    }

    void ClientGameManager::Init()
    {
        //load textures
        if (!trackTexture_.loadFromFile("data/sprites/racetrack.jpg"))
        {
            core::LogError("Could not load track sprite");
        }
        if (!boxTexture_.loadFromFile("data/sprites/Box.png"))
        {
            core::LogError("Could not load box sprite");
        }
        if (!wallTexture_.loadFromFile("data/sprites/wall.png"))
        {
            core::LogError("Could not load wall sprite");
        }
        if (!greatBoxTexture_.loadFromFile("data/sprites/greatbox.png"))
        {
            core::LogError("Could not load greatbox sprite");
        }
        if (!flagTexture_.loadFromFile("data/sprites/flag.png"))
        {
            core::LogError("Could not load flag sprite");
        }
        if (!carTexture_.loadFromFile("data/sprites/car.png"))
        {
            core::LogError("Could not load car sprite");
        }
        //load fonts
        if (!font_.loadFromFile("data/fonts/8-bit-hud.ttf"))
        {
            core::LogError("Could not load font");
        }
        textRenderer_.setFont(font_);
        SpawnLevel();
    }

    void ClientGameManager::Update(sf::Time dt)
    {
        if (state_ & STARTED)
        {
            rollbackManager_.SimulateToCurrentFrame();
            //Copy rollback transform position to our own
            for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
            {
                

                if (entityManager_.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::TRANSFORM)))
                {
                    transformManager_.SetPosition(entity, rollbackManager_.GetTransformManager().GetPosition(entity));
                    transformManager_.SetScale(entity, rollbackManager_.GetTransformManager().GetScale(entity));
                    transformManager_.SetRotation(entity, rollbackManager_.GetTransformManager().GetRotation(entity));
                }
            }
        }
        fixedTimer_ += dt.asSeconds();
        while (fixedTimer_ > FixedPeriod)
        {
            FixedUpdate();
            fixedTimer_ -= FixedPeriod;

        }

    }

    void ClientGameManager::Destroy()
    {
    }

    void ClientGameManager::SetWindowSize(sf::Vector2u windowsSize)
    {
        windowSize_ = windowsSize;
        const sf::FloatRect visibleArea(0, 0, windowSize_.x, windowSize_.y);
        originalView_ = sf::View(visibleArea);
        
        spriteManager_.SetWindowSize(sf::Vector2f(windowsSize));
        spriteManager_.SetCenter(sf::Vector2f(windowsSize) / 2.0f);
    }

    void ClientGameManager::Draw(sf::RenderTarget& target)
    {
        UpdateCameraView();
        target.setView(cameraView_);
        target.clear(sf::Color(0, 128, 0));
        spriteManager_.Draw(target);

        // Draw texts on screen
        target.setView(originalView_);
        if (state_ & FINISHED)
        {
            if (winner_ == GetPlayerNumber())
            {
                const std::string winnerText = fmt::format("You won!");
                textRenderer_.setFillColor(sf::Color::White);
                textRenderer_.setString(winnerText);
                textRenderer_.setCharacterSize(32);
                const auto textBounds = textRenderer_.getLocalBounds();
                textRenderer_.setPosition(windowSize_.x / 2.0f - textBounds.width/2.0f, 
                    windowSize_.y / 2.0f - textBounds.height / 2.0f);
                target.draw(textRenderer_);
            }
            else if (winner_ != INVALID_PLAYER)
            {
                const std::string winnerText = fmt::format("P{} won!", winner_ + 1);
                textRenderer_.setFillColor(sf::Color::White);
                textRenderer_.setString(winnerText);
                textRenderer_.setCharacterSize(32);
                const auto textBounds = textRenderer_.getLocalBounds();
                textRenderer_.setPosition(windowSize_.x / 2.0f - textBounds.width / 2.0f,
                    windowSize_.y / 2.0f - textBounds.height / 2.0f);
                target.draw(textRenderer_);
            }
            else
            {
                const std::string errorMessage = fmt::format("Error with other players");
                textRenderer_.setFillColor(sf::Color::Red);
                textRenderer_.setString(errorMessage);
                textRenderer_.setCharacterSize(32);
                const auto textBounds = textRenderer_.getLocalBounds();
                textRenderer_.setPosition(windowSize_.x / 2.0f - textBounds.width / 2.0f,
                    windowSize_.y / 2.0f - textBounds.height / 2.0f);
                target.draw(textRenderer_);
            }
        }
        if (!(state_ & STARTED))
        {
            if (startingTime_ != 0)
            {
                using namespace std::chrono;
                unsigned long long ms = duration_cast<milliseconds>(
                    system_clock::now().time_since_epoch()
                    ).count();
                if (ms < startingTime_)
                {
                    const std::string countDownText = fmt::format("Starts in {}", ((startingTime_ - ms) / 1000 + 1));
                    textRenderer_.setFillColor(sf::Color::White);
                    textRenderer_.setString(countDownText);
                    textRenderer_.setCharacterSize(32);
                    const auto textBounds = textRenderer_.getLocalBounds();
                    textRenderer_.setPosition(windowSize_.x / 2.0f - textBounds.width / 2.0f,
                        windowSize_.y / 2.0f - textBounds.height / 2.0f);
                    target.draw(textRenderer_);
                }
            }
        }
        else
        {
            std::string health;
            const auto& playerManager = rollbackManager_.GetPlayerCharacterManager();
            for (PlayerNumber playerNumber = 0; playerNumber < maxPlayerNmb; playerNumber++)
            {
                const auto playerEntity = GetEntityFromPlayerNumber(playerNumber);
                if (playerEntity == core::EntityManager::INVALID_ENTITY)
                {
                    continue;
                }
                //health += fmt::format("P{} health: {} ", playerNumber + 1, playerManager.GetComponent(playerEntity).health);
            }
            textRenderer_.setFillColor(sf::Color::White);
            textRenderer_.setString(health);
            textRenderer_.setPosition(10, 10);
            textRenderer_.setCharacterSize(20);
            target.draw(textRenderer_);
        }
        
    }

    void ClientGameManager::SetClientPlayer(PlayerNumber clientPlayer)
    {
        clientPlayer_ = clientPlayer;
    }

    void ClientGameManager::SpawnPlayer(PlayerNumber playerNumber, core::Vec2f position, core::degree_t rotation)
    {
        core::LogDebug(fmt::format("Spawn player: {}", playerNumber));

        GameManager::SpawnPlayer(playerNumber, position, rotation);
        const auto entity = GetEntityFromPlayerNumber(playerNumber);
        entityManager_.AddComponent(entity, static_cast<core::EntityMask>(ComponentType::PLAYER_CHARACTER));
        spriteManager_.AddComponent(entity);
        spriteManager_.SetTexture(entity, carTexture_);
        spriteManager_.SetOrigin(entity, sf::Vector2f(carTexture_.getSize())/2.0f);
        auto sprite = spriteManager_.GetComponent(entity);
        sprite.setColor(playerColors[playerNumber]);
        spriteManager_.SetComponent(entity, sprite);

    }

    core::Entity ClientGameManager::SpawnBox(core::Vec2f position)
    {
        core::LogDebug("ClientSpawnBox");
        const auto boxEntity = GameManager::SpawnBox(position);
        entityManager_.AddComponent(boxEntity, static_cast<core::EntityMask>(ComponentType::WALL));
        spriteManager_.AddComponent(boxEntity);
        spriteManager_.SetTexture(boxEntity, boxTexture_);
        spriteManager_.SetOrigin(boxEntity, sf::Vector2f(boxTexture_.getSize()) / 2.0f);
       
        spriteManager_.SetColor(boxEntity, sf::Color::Black);
        return boxEntity;
       
    }

    core::Entity ClientGameManager::SpawnFlag(core::Vec2f position)
    {
        core::LogDebug("ClientSpawnFlag");
        const auto flagEntity = GameManager::SpawnFlag(position);

        spriteManager_.AddComponent(flagEntity);
        spriteManager_.SetTexture(flagEntity, flagTexture_);
        spriteManager_.SetOrigin(flagEntity, sf::Vector2f(flagTexture_.getSize()) / 2.0f);

        return flagEntity;
    }

    core::Entity ClientGameManager::SpawnTrack(core::Vec2f position)
    {
        core::LogDebug("ClientSpawnTrack");
        const auto trackEntity = GameManager::SpawnTrack(position);

        spriteManager_.AddComponent(trackEntity);
        spriteManager_.SetTexture(trackEntity, trackTexture_);
        spriteManager_.SetOrigin(trackEntity, sf::Vector2f(trackTexture_.getSize()) / 2.0f);

        return trackEntity;
    }

    core::Entity ClientGameManager::SpawnWall(core::Vec2f position)
    {
        core::LogDebug("ClientSpawnWall");
        const auto wallEntity = GameManager::SpawnWall(position);
        entityManager_.AddComponent(wallEntity, static_cast<core::EntityMask>(ComponentType::WALL));
        spriteManager_.AddComponent(wallEntity);
        spriteManager_.SetTexture(wallEntity, wallTexture_);
        spriteManager_.SetOrigin(wallEntity, sf::Vector2f(wallTexture_.getSize()) / 2.0f);

        spriteManager_.SetColor(wallEntity, sf::Color::Black);
        return wallEntity;
    }

    core::Entity ClientGameManager::SpawnGreatBox(core::Vec2f position)
    {
        core::LogDebug("ClientSpawnGreatBox");
        const auto greatBoxEntity = GameManager::SpawnGreatBox(position);
        entityManager_.AddComponent(greatBoxEntity, static_cast<core::EntityMask>(ComponentType::WALL));
        spriteManager_.AddComponent(greatBoxEntity);
        spriteManager_.SetTexture(greatBoxEntity, greatBoxTexture_);
        spriteManager_.SetOrigin(greatBoxEntity, sf::Vector2f(greatBoxTexture_.getSize()) / 2.0f);

        spriteManager_.SetColor(greatBoxEntity, sf::Color::Black);
        return greatBoxEntity;
    }

    void ClientGameManager::FixedUpdate()
    {
        if (!(state_ & STARTED))
        {
            if (startingTime_ != 0)
            {
                using namespace std::chrono;
                const auto ms = duration_cast<duration<unsigned long long, std::milli>>(
                    system_clock::now().time_since_epoch()
                    ).count();
                if (ms > startingTime_)
                {
                    state_ = state_ | STARTED;
                }
                else
                {

                    return;
                }
            }
            else
            {
                return;
            }
        }
        if (state_ & FINISHED)
        {
            return;
        }

        //We send the player inputs when the game started
        const auto playerNumber = GetPlayerNumber();
        if (playerNumber == INVALID_PLAYER)
        {
            //We still did not receive the spawn player packet, but receive the start game packet
            core::LogWarning(fmt::format("Invalid Player Entity in {}:line {}", __FILE__, __LINE__));
            return;
        }
        const auto& inputs = rollbackManager_.GetInputs(playerNumber);
        auto playerInputPacket = std::make_unique<PlayerInputPacket>();
        playerInputPacket->playerNumber = playerNumber;
        playerInputPacket->currentFrame = core::ConvertToBinary(currentFrame_);
        for (size_t i = 0; i < playerInputPacket->inputs.size(); i++)
        {
            if (i > currentFrame_)
            {
                break;
            }

            playerInputPacket->inputs[i] = inputs[i];
        }
        packetSenderInterface_.SendUnreliablePacket(std::move(playerInputPacket));


        currentFrame_++;
        rollbackManager_.StartNewFrame(currentFrame_);
        
        
    }


    void ClientGameManager::SetPlayerInput(PlayerNumber playerNumber, PlayerInput playerInput, std::uint32_t inputFrame)
    {
        if (playerNumber == INVALID_PLAYER)
            return;
        GameManager::SetPlayerInput(playerNumber, playerInput, inputFrame);
    }

    void ClientGameManager::StartGame(unsigned long long int startingTime)
    {
        core::LogDebug(fmt::format("Start game at starting time: {}", startingTime));
        startingTime_ = startingTime;
    }

    void ClientGameManager::DrawImGui()
    {
        ImGui::Text(state_ & STARTED ? "Game has started" : "Game has not started");
        if (startingTime_ != 0)
        {
            ImGui::Text("Starting Time: %llu", startingTime_);
            using namespace std::chrono;
            unsigned long long ms = duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
                ).count();
            ImGui::Text("Current Time: %llu", ms);
        }
    }

    void ClientGameManager::ConfirmValidateFrame(Frame newValidateFrame,
        const std::array<PhysicsState, maxPlayerNmb>& physicsStates)
    {
        if (newValidateFrame < rollbackManager_.GetLastValidateFrame())
        {
            //core::LogDebug(fmt::format("[Warning] New validate frame is too old"));
            return;
        }
        for (PlayerNumber playerNumber = 0; playerNumber < maxPlayerNmb; playerNumber++)
        {
            if (rollbackManager_.GetLastReceivedFrame(playerNumber) < newValidateFrame)
            {
               /*
                core::LogDebug(fmt::format("[Warning] Trying to validate frame {} while playerNumber {} is at input frame {}, client player {}",
                    newValidateFrame,
                    playerNumber + 1,
                    rollbackManager_.GetLastReceivedFrame(playerNumber),
                    GetPlayerNumber()+1));
                */
                return;
                
            }
        }
        rollbackManager_.ConfirmFrame(newValidateFrame, physicsStates);
    }

    void ClientGameManager::WinGame(PlayerNumber winner)
    {
        GameManager::WinGame(winner);
        state_ = state_ | FINISHED;
    }

    void ClientGameManager::UpdateCameraView()
    {
        if(!(state_ & STARTED))
        {
            cameraView_ = originalView_;
            return;
        }

        //the camera stay focused on the player with a constant zoom value
        cameraView_ = originalView_;
        
        const sf::Vector2f extends{ cameraView_.getSize() / 2.0f / PixelPerUnit };
        float currentZoom = 2.5f;
        constexpr float margin = 1.0f;
        const auto playerEntity = GetEntityFromPlayerNumber(clientPlayer_);
        auto playerPos = transformManager_.GetPosition(playerEntity);
        
        playerPos.y = -playerPos.y;
        cameraView_.setCenter((playerPos + extends).toSf() * core::pixelPerMeter);
        cameraView_.zoom(currentZoom);
        
    }
}
