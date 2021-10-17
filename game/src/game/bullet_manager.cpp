#include <game/bullet_manager.h>

namespace game
{
    BulletManager::BulletManager(core::EntityManager& entityManager, GameManager& gameManager) :
        ComponentManager(entityManager), gameManager_(gameManager)
    {
    }

    void BulletManager::FixedUpdate(sf::Time dt)
    {
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::BULLET)))
            {
                auto& bullet = components_[entity];
                bullet.remainingTime -= dt.asSeconds();
                if (bullet.remainingTime < 0.0f)
                {
                    entityManager_.DestroyEntity(entity);
                }
            }
        }
    }
}
