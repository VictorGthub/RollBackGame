#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "client.h"
#include "network_client.h"
#include "engine/system.h"
#include "game/game_globals.h"
#include "graphics/graphics.h"
#include "maths/vec2.h"


namespace game
{

class NetworkDebugApp : public core::DrawInterface, public core::DrawImGuiInterface, public core::SystemInterface, public core::OnEventInterface
    {
    public:
        NetworkDebugApp();

        void Init() override;

        void Update(sf::Time dt) override;

        void Destroy() override;

        void DrawImGui() override;

        void Draw(sf::RenderTarget& window) override;

        void OnEvent(const sf::Event& event) override;
    private:
        std::array<ClientNetworkManager, maxPlayerNmb> clients_;
        std::array<sf::RenderTexture, maxPlayerNmb> clientsFramebuffers_;
        sf::Sprite screenQuad_;
        sf::Vector2u windowSize_;
    };


}
