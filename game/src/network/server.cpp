#include <network/server.h>
#include <utils/log.h>
#include <fmt/format.h>
#include <utils/conversion.h>
#include <cstdint>

namespace game
{

    void Server::ReceivePacket(std::unique_ptr<Packet> packet)
    {
        const auto packetType = static_cast<PacketType>(packet->packetType);
        switch (packetType)
        {
        case PacketType::JOIN:
        {
            const auto* joinPacket = static_cast<const JoinPacket*>(packet.get());
            const auto clientId = core::ConvertFromBinary<ClientId>(joinPacket->clientId);
            if (std::find(clientMap_.begin(), clientMap_.end(), clientId) != clientMap_.end())
            {
                //Player joined twice!
                return;
            }
            core::LogDebug("Managing Received Packet Join from: " + std::to_string(clientId));
            clientMap_[lastPlayerNumber_] = clientId;
            SpawnNewPlayer(clientId, lastPlayerNumber_);

            lastPlayerNumber_++;

            if (lastPlayerNumber_ == maxPlayerNmb)
            {
                auto startGamePacket = std::make_unique<StartGamePacket>();
                startGamePacket->packetType = PacketType::START_GAME;
                using namespace std::chrono;
                const auto ms = (duration_cast<duration<unsigned long long, std::milli>>(
                    system_clock::now().time_since_epoch()
                    ) + milliseconds(3000)).count();
                startGamePacket->startTime = core::ConvertToBinary(ms);
                SendReliablePacket(std::move(startGamePacket));
            }

            break;
        }
        case PacketType::INPUT:
        {
            //Manage internal state
            const auto* playerInputPacket = static_cast<const PlayerInputPacket*>(packet.get());
            const auto playerNumber = playerInputPacket->playerNumber;
            const auto inputFrame = core::ConvertFromBinary<Frame>(playerInputPacket->currentFrame);

            for (std::uint32_t i = 0; i < playerInputPacket->inputs.size(); i++)
            {
                gameManager_.SetPlayerInput(playerNumber,
                    playerInputPacket->inputs[i],
                    inputFrame - i);
                if (inputFrame - i == 0)
                {
                    break;
                }
            }

            SendUnreliablePacket(std::move(packet));

            //Validate new frame if needed
            std::uint32_t lastReceiveFrame = gameManager_.GetRollbackManager().GetLastReceivedFrame(0);
            for (PlayerNumber i = 1; i < maxPlayerNmb; i++)
            {
                const auto playerLastFrame = gameManager_.GetRollbackManager().GetLastReceivedFrame(i);
                if (playerLastFrame < lastReceiveFrame)
                {
                    lastReceiveFrame = playerLastFrame;
                }
            }
            if (lastReceiveFrame > gameManager_.GetLastValidateFrame())
            {
                //Validate frame
                gameManager_.Validate(lastReceiveFrame);

                auto validatePacket = std::make_unique<ValidateFramePacket>();
                validatePacket->newValidateFrame = core::ConvertToBinary(lastReceiveFrame);

                //copy physics state
                for (PlayerNumber i = 0; i < maxPlayerNmb; i++)
                {
                    auto physicsState = gameManager_.GetRollbackManager().GetValidatePhysicsState(i);
                    const auto* statePtr = reinterpret_cast<const std::uint8_t*>(&physicsState);
                    for (size_t j = 0; j < sizeof(PhysicsState); j++)
                    {
                        validatePacket->physicsState[i * sizeof(PhysicsState) + j] = statePtr[j];
                    }
                }
                SendUnreliablePacket(std::move(validatePacket));
                const auto winner = gameManager_.CheckWinner();
                if (winner != INVALID_PLAYER)
                {
                    core::LogDebug(fmt::format("Server declares P{} a winner", winner + 1));
                    auto winGamePacket = std::make_unique<WinGamePacket>();
                    winGamePacket->winner = winner;
                    SendReliablePacket(std::move(winGamePacket));
                    gameManager_.WinGame(winner);
                }
            }

            break;
        }
        default: break;
        }
    }
}
