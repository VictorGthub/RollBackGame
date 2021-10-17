#pragma once
#include <SFML/System/Time.hpp>

#include "game_globals.h"

namespace game
{
    struct Bullet
    {
        float remainingTime = 0.0f;
        PlayerNumber playerNumber = INVALID_PLAYER;
    };

    class GameManager;
    class BulletManager : public core::ComponentManager<Bullet, static_cast<core::EntityMask>(ComponentType::BULLET)>
    {
    public:
        explicit BulletManager(core::EntityManager& entityManager, GameManager& gameManager);
        void FixedUpdate(sf::Time dt);
    private:
        GameManager& gameManager_;
    };
}
