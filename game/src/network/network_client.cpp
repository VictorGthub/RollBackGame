#include <imgui.h>
#include <network/network_client.h>

#include "maths/basic.h"
#include "utils/conversion.h"
#include "utils/log.h"

namespace game
{

    void ClientNetworkManager::Init()
    {

        clientId_ = core::RandomRange(std::numeric_limits<ClientId>::lowest(),
                                      std::numeric_limits<ClientId>::max());
        //JOIN packet
        gameManager_.Init();
        tcpSocket_.setBlocking(false);
        udpSocket_.setBlocking(true);
        auto status = sf::Socket::Error;
        while (status != sf::Socket::Done)
        {
            status = udpSocket_.bind(sf::Socket::AnyPort);
        }
        udpSocket_.setBlocking(false);
    }

    void ClientNetworkManager::Update(sf::Time dt)
    {

        if (currentState_ != State::NONE)
        {
            auto status = sf::Socket::Done;
            //Receive TCP Packet
            while (status == sf::Socket::Done)
            {
                sf::Packet packet;
                status = tcpSocket_.receive(packet);
                switch (status)
                {
                case sf::Socket::Done:
                    ReceivePacket(packet, PacketSource::TCP);
                    break;
                case sf::Socket::NotReady:
                    //core::LogDebug("[Client] Error while receiving tcp socket is not ready");
                    break;
                case sf::Socket::Partial:
                    core::LogDebug("[Client] Error while receiving TCP packet, PARTIAL");
                    break;
                case sf::Socket::Disconnected: break;
                case sf::Socket::Error: break;
                default:;
                }
            }
            //Receive UDP packet
            status = sf::Socket::Done;
            while (status == sf::Socket::Done)
            {
                sf::Packet packet;
                sf::IpAddress sender;
                unsigned short port;
                status = udpSocket_.receive(packet, sender, port);
                switch (status)
                {
                case sf::Socket::Done:
                    ReceivePacket(packet, PacketSource::UDP);
                    break;
                case sf::Socket::NotReady: break;
                case sf::Socket::Partial:
                    core::LogDebug("[Client] Error while receiving UDP packet, PARTIAL");
                    break;
                case sf::Socket::Disconnected:
                    core::LogDebug("[Client] Error while receiving UDP packet, DISCONNECTED");
                    break;
                case sf::Socket::Error:
                    core::LogDebug("[Client] Error while receiving UDP packet, ERROR");
                    break;
                default:;
                }
            }
            switch (currentState_)
            {
            case State::JOINING:
            {
                if (serverUdpPort_ != 0)
                {
                    //Need to send a join packet on the unreliable channel
                    auto joinPacket = std::make_unique<JoinPacket>();
                    joinPacket->clientId = core::ConvertToBinary<ClientId>(clientId_);
                    SendUnreliablePacket(std::move(joinPacket));
                }
                break;
            }
            default:
                break;
            }
        }

        gameManager_.Update(dt);
    }

    void ClientNetworkManager::Destroy()
    {
        gameManager_.Destroy();
    }

    void ClientNetworkManager::DrawImGui()
    {
        const auto windowName = "Client " + std::to_string(clientId_);
        ImGui::Begin(windowName.c_str());
        const size_t bufferSize = 100;
        char hostBuffer[bufferSize];
        std::strncpy(hostBuffer, serverAddress_.c_str(), bufferSize);
        if (ImGui::InputText("Host", hostBuffer, bufferSize))
        {
            serverAddress_ = hostBuffer;
        }
        int portBuffer = serverTcpPort_;
        if (ImGui::InputInt("Port", &portBuffer))
        {
            serverTcpPort_ = static_cast<unsigned short>(portBuffer);
        }
        if (currentState_ == State::NONE &&
            ImGui::Button("Join"))
        {
            tcpSocket_.setBlocking(true);
            const auto status = tcpSocket_.connect(serverAddress_, serverTcpPort_);
            tcpSocket_.setBlocking(false);
            if (status == sf::Socket::Done)
            {
                core::LogDebug("[Client] Connect to server " + serverAddress_ + " with port: " + std::to_string(serverTcpPort_));
                auto joinPacket = std::make_unique<JoinPacket>();
                joinPacket->clientId = core::ConvertToBinary<ClientId>(clientId_);
                using namespace std::chrono;
                const unsigned long clientTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
                joinPacket->startTime = core::ConvertToBinary<unsigned long>(clientTime);
                SendReliablePacket(std::move(joinPacket));
                currentState_ = State::JOINING;
            }
            else
            {
                core::LogDebug("[Client] Error trying to connect to " + serverAddress_ + " with port: " +
                    std::to_string(serverTcpPort_) + " with status: " + std::to_string(status));
            }
        }
        ImGui::Text("Server UDP port: %u", serverUdpPort_);
        gameManager_.DrawImGui();
        ImGui::End();
    }

    void ClientNetworkManager::Draw(sf::RenderTarget& renderTarget)
    {
        gameManager_.Draw(renderTarget);
    }

    void ClientNetworkManager::SendReliablePacket(std::unique_ptr<Packet> packet)
    {

        //core::LogDebug("[Client] Sending reliable packet to server");
        sf::Packet tcpPacket;
        GeneratePacket(tcpPacket, *packet);
        auto status = sf::Socket::Partial;
        while (status == sf::Socket::Partial)
        {
            status = tcpSocket_.send(tcpPacket);
        }
    }

    void ClientNetworkManager::SendUnreliablePacket(std::unique_ptr<Packet> packet)
    {

        sf::Packet udpPacket;
        GeneratePacket(udpPacket, *packet);
        const auto status = udpSocket_.send(udpPacket, serverAddress_, serverUdpPort_);
        switch (status)
        {
        case sf::Socket::Done:
            //core::LogDebug("[Client] Sending UDP packet to server at host: " +
            //	serverAddress_.toString() + " port: " + std::to_string(serverUdpPort_));
            break;
        case sf::Socket::NotReady:
            core::LogDebug("[Client] Error sending UDP to server, NOT READY");
            break;
        case sf::Socket::Partial:
            core::LogDebug("[Client] Error sending UDP to server, PARTIAL");
            break;
        case sf::Socket::Disconnected:
            core::LogDebug("[Client] Error sending UDP to server, DISCONNECTED");
            break;
        case sf::Socket::Error:
            core::LogDebug("[Client] Error sending UDP to server, ERROR");
            break;
        default:
            break;
        }
    }

    void ClientNetworkManager::SetPlayerInput(PlayerInput input)
    {
        const auto currentFrame = gameManager_.GetCurrentFrame();
        gameManager_.SetPlayerInput(
            gameManager_.GetPlayerNumber(),
            input,
            currentFrame);
    }

    void ClientNetworkManager::ReceivePacket(sf::Packet& packet, PacketSource source)
    {
        const auto receivePacket = GenerateReceivedPacket(packet);
        Client::ReceivePacket(receivePacket.get());
        switch (receivePacket->packetType)
        {
        case PacketType::JOIN_ACK:
        {
            core::LogDebug("[Client] Receive " + std::string(source == PacketSource::UDP ? "UDP" : "TCP") + " Join ACK Packet");
            auto* joinAckPacket = static_cast<JoinAckPacket*>(receivePacket.get());

            serverUdpPort_ = core::ConvertFromBinary<unsigned short>(joinAckPacket->udpPort);
            const auto clientId = core::ConvertFromBinary<ClientId>(joinAckPacket->clientId);
            if (clientId != clientId_)
                return;
            if (source == PacketSource::TCP)
            {
                //Need to send a join packet on the unreliable channel
                auto joinPacket = std::make_unique<JoinPacket>();
                joinPacket->clientId = core::ConvertToBinary<ClientId>(clientId_);
                SendUnreliablePacket(std::move(joinPacket));
            }
            else
            {
                if (currentState_ == State::JOINING)
                {
                    currentState_ = State::JOINED;
                }
            }
            break;
        }
        default:
            break;
        }
    }
}
