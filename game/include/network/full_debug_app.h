#pragma once
#include "client.h"
#include "simulation_client.h"
#include "simulation_server.h"
#include "engine/system.h"
#include "game/game_globals.h"
#include "graphics/graphics.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace game
{
    class SimulationDebugApp : public core::SystemInterface, public core::DrawInterface, public core::DrawImGuiInterface, public core::OnEventInterface
    {
    public:
        SimulationDebugApp();

        void Init() override;

        void Update(sf::Time dt) override;

        void Destroy() override;

        void DrawImGui() override;

        void Draw(sf::RenderTarget& window) override;

        void OnEvent(const sf::Event& event) override;
    private:
        std::array<std::unique_ptr<SimulationClient>, maxPlayerNmb> clients_;
        std::array<sf::RenderTexture, maxPlayerNmb> clientsFramebuffers_;
        SimulationServer server_;
        sf::Sprite screenQuad_;
        sf::Vector2u windowSize_;
    };
}
