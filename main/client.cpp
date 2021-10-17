
#include "engine/engine.h"
#include "engine/system.h"
#include "graphics/graphics.h"
#include "network/network_client.h"

namespace game
{

    class ClientApp : public core::SystemInterface, public core::DrawImGuiInterface, public core::DrawInterface, public core::OnEventInterface
    {
    public:
        void Init() override
        {
            windowSize_ = core::windowSize;
            client_.SetWindowSize(windowSize_);
            client_.Init();
        }

        void Update(sf::Time dt) override
        {
            PlayerInput clientInput1 = 0;
            clientInput1 = clientInput1 | (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ? PlayerInputEnum::PlayerInput::LEFT : 0u);
            clientInput1 = clientInput1 | (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ? PlayerInputEnum::PlayerInput::RIGHT : 0u);
            clientInput1 = clientInput1 | (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ? PlayerInputEnum::PlayerInput::UP : 0u);
            clientInput1 = clientInput1 | (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ? PlayerInputEnum::PlayerInput::DOWN : 0u);
            clientInput1 = clientInput1 | (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ? PlayerInputEnum::PlayerInput::SHOOT : 0u);

            client_.SetPlayerInput(clientInput1);
            client_.Update(dt);
        }

        void Destroy() override
        {
            client_.Destroy();
        }

        void DrawImGui() override
        {
            client_.DrawImGui();
        }

        void OnEvent(const sf::Event& event) override
        {
            switch (event.type)
            {
            case sf::Event::Resized:
            {
                windowSize_ = sf::Vector2u(event.size.width, event.size.height);
                client_.SetWindowSize(windowSize_);
                break;
            }
            default:;
            }
        }
        void Draw(sf::RenderTarget& window) override
        {
            client_.Draw(window);
        }

    private:
        sf::Vector2u windowSize_;
        ClientNetworkManager client_;
    };
}

int main()
{
    core::Engine engine;
    game::ClientApp app;
    engine.RegisterSystem(&app);
    engine.RegisterDraw(&app);
    engine.RegisterDrawImGui(&app);
    engine.RegisterOnEvent(&app);

    engine.Run();
    return 0;
}