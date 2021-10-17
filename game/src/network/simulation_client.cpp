#include <network/simulation_client.h>
#include <maths/basic.h>
#include <imgui.h>
#include <network/simulation_server.h>

namespace game
{
    SimulationClient::SimulationClient(SimulationServer& server) :
        server_(server)
    {
    }

    void SimulationClient::Init()
    {
        
        clientId_ = core::RandomRange(std::numeric_limits<ClientId>::lowest(),
                                      std::numeric_limits<ClientId>::max());
        //JOIN packet
        gameManager_.Init();


    }

    void SimulationClient::Update(sf::Time dt)
    {
        gameManager_.Update(dt);
    }



    void SimulationClient::Destroy()
    {
        gameManager_.Destroy();

    }

    void SimulationClient::Draw(sf::RenderTarget& renderTarget)
    {
        gameManager_.Draw(renderTarget);
    }


    void SimulationClient::SetPlayerInput(PlayerInput playerInput)
    {
        const auto currentFrame = gameManager_.GetCurrentFrame();
        gameManager_.SetPlayerInput(
            gameManager_.GetPlayerNumber(),
            playerInput,
            currentFrame);

    }

    void SimulationClient::DrawImGui()
    {
        const auto windowName = "Client " + std::to_string(clientId_);
        ImGui::Begin(windowName.c_str());
        if (gameManager_.GetPlayerNumber() == INVALID_PLAYER && ImGui::Button("Spawn Player"))
        {
            auto joinPacket = std::make_unique<JoinPacket>();
            const auto* clientIdPtr = reinterpret_cast<std::uint8_t*>(&clientId_);
            for (std::size_t i = 0; i < sizeof(clientId_); i++)
            {
                joinPacket->clientId[i] = clientIdPtr[i];
            }
            SendReliablePacket(std::move(joinPacket));
        }
        gameManager_.DrawImGui();
        ImGui::End();
    }

    void SimulationClient::SendUnreliablePacket(std::unique_ptr<Packet> packet)
    {
        server_.PutPacketInReceiveQueue(std::move(packet));
    }

    void SimulationClient::SendReliablePacket(std::unique_ptr<Packet> packet)
    {
        server_.PutPacketInReceiveQueue(std::move(packet));
    }

}
