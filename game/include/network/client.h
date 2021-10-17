#pragma once
#include "packet_type.h"
#include "game/game_manager.h"
#include "graphics/graphics.h"

namespace game
{
    class Client : public core::DrawInterface, public core::DrawImGuiInterface, public PacketSenderInterface, public core::SystemInterface
    {
    public:
        Client() : gameManager_(*this)
        {

        }
        virtual ~Client() = default;
        virtual void SetWindowSize(sf::Vector2u windowSize)
        {
            gameManager_.SetWindowSize(windowSize);
        }
        virtual void ReceivePacket(const Packet* packet);
    protected:

        ClientGameManager gameManager_;
        ClientId clientId_ = 0;
    };
}
