#pragma once
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpListener.hpp>

#include "network_client.h"
#include "server.h"
#include "game/game_globals.h"

namespace game
{
    struct ClientInfo
    {
        ClientId clientId = 0;
        unsigned long long timeDifference = 0;
        sf::IpAddress udpRemoteAddress;
        unsigned short udpRemotePort = 0;


    };
    class ServerNetworkManager : public Server
    {
    public:
        enum class PacketSocketSource
        {
            TCP,
            UDP
        };
        void SendReliablePacket(std::unique_ptr<Packet> packet) override;

        void SendUnreliablePacket(std::unique_ptr<Packet> packet) override;

        void Init() override;

        void Update(sf::Time dt) override;

        void Destroy() override;

        void SetTcpPort(unsigned short i);

        bool IsOpen() const;
    protected:
        void SpawnNewPlayer(ClientId clientId, PlayerNumber playerNumber) override;

    private:
        void ProcessReceivePacket(std::unique_ptr<Packet> packet,
            PacketSocketSource packetSource,
            sf::IpAddress address = "localhost",
            unsigned short port = 0);
        void ReceivePacket(sf::Packet& packet, PacketSocketSource packetSource,
            sf::IpAddress address = "localhost",
            unsigned short port = 0);

        enum ServerStatus
        {
            OPEN = 1u << 0u,
            STARTED = 1u << 1u,
            FIRST_PLAYER_CONNECT = 1u << 2u,
        };
        sf::UdpSocket udpSocket_;
        sf::TcpListener tcpListener_;
        std::array<sf::TcpSocket, maxPlayerNmb> tcpSockets_;

        std::array<ClientInfo, maxPlayerNmb> clientInfoMap_{};


        unsigned short tcpPort_ = 12345;
        unsigned short udpPort_ = 12345;
        std::uint32_t lastSocketIndex_ = 0;
        std::uint8_t status_ = 0;
    };
}
