#include <network/network_server.h>
#include <utils/log.h>
#include <fmt/format.h>
#include <utils/conversion.h>
#include <cassert>

namespace game
{
    void ServerNetworkManager::SendReliablePacket(
        std::unique_ptr<Packet> packet)
    {
        core::LogDebug(fmt::format("[Server] Sending TCP packet: {}",
            std::to_string(static_cast<int>(packet->packetType))));
        for (PlayerNumber playerNumber = 0; playerNumber < maxPlayerNmb;
            playerNumber++)
        {
            sf::Packet sendingPacket;
            GeneratePacket(sendingPacket, *packet);

            auto status = sf::Socket::Partial;
            while (status == sf::Socket::Partial)
            {
                status = tcpSockets_[playerNumber].send(sendingPacket);
                switch (status)
                {
                case sf::Socket::NotReady:
                    core::LogDebug(fmt::format(
                        "[Server] Error trying to send packet to Player: {} socket is not ready",
                        playerNumber));
                    break;
                case sf::Socket::Disconnected:

                    break;
                }
            }
        }
    }

    void ServerNetworkManager::SendUnreliablePacket(
        std::unique_ptr<Packet> packet)
    {
        for (PlayerNumber playerNumber = 0; playerNumber < maxPlayerNmb;
            playerNumber++)
        {
            if (clientInfoMap_[playerNumber].udpRemotePort == 0)
            {
                core::LogDebug(fmt::format("[Warning] Trying to send UDP packet, but missing port!"));
                continue;
            }

            sf::Packet sendingPacket;
            GeneratePacket(sendingPacket, *packet);
            const auto status = udpSocket_.send(sendingPacket, clientInfoMap_[playerNumber].udpRemoteAddress,
                clientInfoMap_[playerNumber].udpRemotePort);
            switch (status)
            {
            case sf::Socket::Done:
                //core::LogDebug("[Server] Sending UDP packet: " +
                    //std::to_string(static_cast<int>(packet->packetType)));
                break;

            case sf::Socket::Disconnected:
            {
                core::LogDebug("[Server] Error while sending UDP packet, DISCONNECTED");
                break;
            }
            case sf::Socket::NotReady:
                core::LogDebug("[Server] Error while sending UDP packet, NOT READY");

                break;

            case sf::Socket::Error:
                core::LogDebug("[Server] Error while sending UDP packet, DISCONNECTED");
                break;
            default:
                break;
            }

        }

    }

    void ServerNetworkManager::Init()
    {
        sf::Socket::Status status = sf::Socket::Error;
        while (status != sf::Socket::Done)
        {
            // bind the listener to a port
            status = tcpListener_.listen(tcpPort_);
            if (status != sf::Socket::Done)
            {
                tcpPort_++;
            }
        }
        tcpListener_.setBlocking(false);
        for (auto& socket : tcpSockets_)
        {
            socket.setBlocking(false);
        }
        core::LogDebug(fmt::format("[Server] Tcp Socket on port: {}", tcpPort_));

        status = sf::Socket::Error;
        while (status != sf::Socket::Done)
        {
            status = udpSocket_.bind(udpPort_);
            if (status != sf::Socket::Done)
            {
                udpPort_++;
            }
        }
        udpSocket_.setBlocking(false);
        core::LogDebug(fmt::format("[Server] Udp Socket on port: {}", udpPort_));

        status_ = status_ | OPEN;

    }

    void ServerNetworkManager::Update(sf::Time dt)
    {
        if (lastSocketIndex_ < maxPlayerNmb)
        {
            const sf::Socket::Status status = tcpListener_.accept(
                tcpSockets_[lastSocketIndex_]);
            if (status == sf::Socket::Done)
            {
                const auto remoteAddress = tcpSockets_[lastSocketIndex_].
                    getRemoteAddress();
                core::LogDebug(fmt::format("[Server] New player connection with address: {} and port: {}",
                    remoteAddress.toString(), tcpSockets_[lastSocketIndex_].getRemotePort()));
                status_ = status_ | (FIRST_PLAYER_CONNECT << lastSocketIndex_);
                lastSocketIndex_++;
            }
        }

        for (PlayerNumber playerNumber = 0; playerNumber < maxPlayerNmb;
            playerNumber++)
        {
            sf::Packet tcpPacket;
            const auto status = tcpSockets_[playerNumber].receive(
                tcpPacket);
            switch (status)
            {
            case sf::Socket::Done:
                ReceivePacket(tcpPacket, PacketSocketSource::TCP);
                break;
            case sf::Socket::Disconnected:
            {
                core::LogDebug(fmt::format(
                    "[Error] Player Number {} is disconnected when receiving",
                    playerNumber + 1));
                status_ = status_ & ~(FIRST_PLAYER_CONNECT << playerNumber);
                auto endGame = std::make_unique<WinGamePacket>();
                SendReliablePacket(std::move(endGame));
                status_ = status_ & ~OPEN; //Close the server
                break;
            }
            default: break;
            }
        }
        sf::Packet udpPacket;
        sf::IpAddress address;
        unsigned short port;
        const auto status = udpSocket_.receive(udpPacket, address, port);
        if (status == sf::Socket::Done)
        {
            ReceivePacket(udpPacket, PacketSocketSource::UDP, address, port);
        }
    }

    void ServerNetworkManager::Destroy()
    {

    }

    void ServerNetworkManager::SetTcpPort(unsigned short i)
    {
        tcpPort_ = i;
    }

    bool ServerNetworkManager::IsOpen() const
    {
        return status_ & OPEN;
    }

    void ServerNetworkManager::SpawnNewPlayer(ClientId clientId, PlayerNumber playerNumber)
    {
        //Spawning the new player in the arena
        for (PlayerNumber p = 0; p <= lastPlayerNumber_; p++)
        {
            auto spawnPlayer = std::make_unique<SpawnPlayerPacket>();
            spawnPlayer->clientId = core::ConvertToBinary(clientMap_[p]);
            spawnPlayer->playerNumber = p;

            const auto pos = spawnPositions[p] * 3.0f;
            spawnPlayer->pos = ConvertToBinary(pos);

            const auto rotation = spawnRotations[p];
            spawnPlayer->angle = core::ConvertToBinary(rotation);
            gameManager_.SpawnPlayer(p, pos, rotation);

            SendReliablePacket(std::move(spawnPlayer));
        }
    }


    void ServerNetworkManager::ProcessReceivePacket(
        std::unique_ptr<Packet> packet,
        PacketSocketSource packetSource,
        sf::IpAddress address,
        unsigned short port)
    {

        const auto packetType = static_cast<PacketType>(packet->packetType);
        switch (packetType)
        {
        case PacketType::JOIN:
        {
            const auto joinPacket = *static_cast<JoinPacket*>(packet.get());
            Server::ReceivePacket(std::move(packet));
            auto clientId = core::ConvertFromBinary<ClientId>(joinPacket.clientId);
            core::LogDebug(fmt::format("[Server] Received Join Packet from: {} {}", clientId,
                (packetSource == PacketSocketSource::UDP ? fmt::format(" UDP with port: {}", port) : " TCP")));
            const auto it = std::find(clientMap_.begin(), clientMap_.end(), clientId);
            PlayerNumber playerNumber;
            if (it != clientMap_.end())
            {
                playerNumber = std::distance(clientMap_.begin(), it);
                clientInfoMap_[playerNumber].clientId = clientId;
            }
            else
            {
                assert(false && "Player Number is supposed to be already set!");
            }

            auto joinAckPacket = std::make_unique<JoinAckPacket>();
            joinAckPacket->clientId = core::ConvertToBinary(clientId);
            joinAckPacket->udpPort = core::ConvertToBinary(udpPort_);
            if (packetSource == PacketSocketSource::UDP)
            {
                auto& clientInfo = clientInfoMap_[playerNumber];
                clientInfo.udpRemoteAddress = address;
                clientInfo.udpRemotePort = port;
                SendUnreliablePacket(std::move(joinAckPacket));
            }
            else
            {
                SendReliablePacket(std::move(joinAckPacket));
                //Calculate time difference
                const auto clientTime = core::ConvertFromBinary<unsigned long>(joinPacket.startTime);
                using namespace std::chrono;
                const unsigned long deltaTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()) - clientTime;
                core::LogDebug(fmt::format("[Server] Client Server deltaTime: {}", deltaTime));
                clientInfoMap_[playerNumber].timeDifference = deltaTime;
            }
            break;
        }
        default:
            Server::ReceivePacket(std::move(packet));
            break;
        }
    }

    void ServerNetworkManager::ReceivePacket(sf::Packet& packet,
        PacketSocketSource packetSource,
        sf::IpAddress address,
        unsigned short port)
    {
        auto receivedPacket = GenerateReceivedPacket(packet);

        if (receivedPacket != nullptr)
        {
            ProcessReceivePacket(std::move(receivedPacket), packetSource, address, port);
        }
    }
}
