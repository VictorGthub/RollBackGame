#pragma once
#include <memory>
#include <network/client.h>
#include <SFML/System/Time.hpp>

namespace game
{
    class SimulationServer;

    class SimulationClient : public Client
    {
    public:
        explicit SimulationClient(SimulationServer& server);

        void Init() override;
        void Update(sf::Time dt) override;

        void Destroy() override;
        void Draw(sf::RenderTarget& window) override;


        void SendUnreliablePacket(std::unique_ptr<Packet> packet) override;
        void SendReliablePacket(std::unique_ptr<Packet> packet) override;


        void DrawImGui() override;
        void SetPlayerInput(PlayerInput input);
        
    private:
        SimulationServer& server_;

    };
}
