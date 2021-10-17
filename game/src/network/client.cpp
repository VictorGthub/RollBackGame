#include <network/client.h>
#include <utils/conversion.h>
#include <cassert>

namespace game
{
    void Client::ReceivePacket(const Packet* packet)
    {
        const auto packetType = packet->packetType;
        switch (packetType)
        {
        case PacketType::SPAWN_PLAYER:
        {
            const auto* spawnPlayerPacket = static_cast<const SpawnPlayerPacket*>(packet);
            const auto clientId = core::ConvertFromBinary<ClientId>(spawnPlayerPacket->clientId);

            const PlayerNumber playerNumber = spawnPlayerPacket->playerNumber;
            if (clientId == clientId_)
            {
                gameManager_.SetClientPlayer(playerNumber);
            }

            const auto pos = core::ConvertFromBinary<core::Vec2f>(spawnPlayerPacket->pos);
            const auto rotation = core::ConvertFromBinary<core::degree_t>(spawnPlayerPacket->angle);

            gameManager_.SpawnPlayer(playerNumber, pos, rotation);
            break;
        }
        case PacketType::START_GAME:
        {
            const auto* startGamePacket = static_cast<const StartGamePacket*>(packet);
            const auto startingTime = core::ConvertFromBinary<unsigned long long>(startGamePacket->startTime);
            gameManager_.StartGame(startingTime);
            break;
        }
        case PacketType::INPUT:
        {
            const auto* playerInputPacket = static_cast<const PlayerInputPacket*>(packet);
            const auto playerNumber = playerInputPacket->playerNumber;
            const auto inputFrame = core::ConvertFromBinary<Frame>(playerInputPacket->currentFrame);

            if (playerNumber == gameManager_.GetPlayerNumber())
            {
                //Verify the inputs coming back from the server
                const auto& inputs = gameManager_.GetRollbackManager().GetInputs(playerNumber);
                const auto currentFrame = gameManager_.GetRollbackManager().GetCurrentFrame();
                for (size_t i = 0; i < playerInputPacket->inputs.size(); i++)
                {
                    const auto index = currentFrame - inputFrame + i;
                    if (index > inputs.size())
                    {
                        break;
                    }
                    if (inputs[index] != playerInputPacket->inputs[i])
                    {
                        assert(false && "Inputs coming back from server are not coherent!!!");
                    }
                    if (inputFrame - i == 0)
                    {
                        break;
                    }
                }
                break;
            }

            //discard delayed input packet
            if (inputFrame < gameManager_.GetRollbackManager().GetLastReceivedFrame(playerNumber))
            {
                break;
            }
            for (Frame i = 0; i < playerInputPacket->inputs.size(); i++)
            {
                gameManager_.SetPlayerInput(playerNumber,
                    playerInputPacket->inputs[i],
                    inputFrame - i);

                if (inputFrame - i == 0)
                {
                    break;
                }
            }
            break;
        }
        case PacketType::VALIDATE_STATE:
        {
            const auto* validateFramePacket = static_cast<const ValidateFramePacket*>(packet);
            const auto newValidateFrame = core::ConvertFromBinary<Frame>(validateFramePacket->newValidateFrame);
            std::array<PhysicsState, maxPlayerNmb> physicsStates{};
            for (size_t i = 0; i < validateFramePacket->physicsState.size(); i++)
            {
                auto* statePtr = reinterpret_cast<std::uint8_t*>(physicsStates.data());
                statePtr[i] = validateFramePacket->physicsState[i];
            }
            gameManager_.ConfirmValidateFrame(newValidateFrame, physicsStates);
            //logDebug("Client received validate frame " + std::to_string(newValidateFrame));
            break;
        }
        case PacketType::WIN_GAME:
        {
            const auto* winGamePacket = static_cast<const WinGamePacket*>(packet);
            gameManager_.WinGame(winGamePacket->winner);
            break;
        }
        case PacketType::SPAWN_BULLET: break;
        default:;
        }

    }
}
