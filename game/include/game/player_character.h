#pragma once
#include <SFML/System/Time.hpp>

#include "game_globals.h"

namespace game
{
    class PhysicsManager;

    struct PlayerCharacter
    {
        PlayerInput input = 0u;
        PlayerNumber playerNumber = INVALID_PLAYER;
        static constexpr float maxSpeed = 3.0f;
        int player1win = 0;
        int player2win = 0;
    };
    class GameManager;
    class PlayerCharacterManager : public core::ComponentManager<PlayerCharacter, core::EntityMask(ComponentType::PLAYER_CHARACTER)>
    {
    public:
        explicit PlayerCharacterManager(core::EntityManager& entityManager, PhysicsManager& physicsManager, GameManager& gameManager);
        void FixedUpdate(sf::Time dt);


    private:
        PhysicsManager& physicsManager_;
        GameManager& gameManager_;
    };
}
