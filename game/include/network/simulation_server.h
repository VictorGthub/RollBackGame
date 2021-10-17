#pragma once
#include <memory>
#include <SFML/System/Time.hpp>

#include "server.h"

namespace game
{
	struct DelayPacket
	{
		float currentTime = 0.0f;
		std::unique_ptr<game::Packet> packet = nullptr;
	};
	class SimulationClient;
	class SimulationServer : public Server, public core::DrawImGuiInterface
	{
	public:
		explicit SimulationServer(std::array<std::unique_ptr<SimulationClient>, 2>& clients);
		void Init() override;
		void Update(sf::Time dt) override;
		void Destroy() override;
		void DrawImGui() override;
		void PutPacketInReceiveQueue(std::unique_ptr<Packet> packet);
		void SendReliablePacket(std::unique_ptr<Packet> packet) override;
		void SendUnreliablePacket(std::unique_ptr<Packet> packet) override;
	private:
		void PutPacketInSendingQueue(std::unique_ptr<Packet> packet);
		void ProcessReceivePacket(std::unique_ptr<Packet> packet);

		void SpawnNewPlayer(ClientId clientId, PlayerNumber playerNumber) override;

		std::vector<DelayPacket> receivedPackets_;
		std::vector<DelayPacket> sentPackets_;
		std::array<std::unique_ptr<SimulationClient>, maxPlayerNmb>& clients_;


		float avgDelay_ = 0.25f;
		float marginDelay_ = 0.1f;
	};
}
