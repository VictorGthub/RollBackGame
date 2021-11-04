#include <game/physics_manager.h>
#include "utils/log.h"

namespace game
{

    PhysicsManager::PhysicsManager(core::EntityManager& entityManager) :
        boxbodyManager_(entityManager), entityManager_(entityManager)
    {

    }

    bool Box2Box(float r1x, float r1y, float r1w, float r1h, float r2x, float r2y, float r2w, float r2h)
    {
        return r1x + r1w >= r2x &&    // r1 right edge past r2 left
            r1x <= r2x + r2w &&    // r1 left edge past r2 right
            r1y + r1h >= r2y &&    // r1 top edge past r2 bottom
            r1y <= r2y + r2h;
    }

    void PhysicsManager::FixedUpdate(sf::Time dt)
    {
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::BOXBODY2D)))
                continue;
            auto body = boxbodyManager_.GetComponent(entity);
            body.position += body.velocity * dt.asSeconds();
            body.rotation += body.angularVelocity * dt.asSeconds();
            boxbodyManager_.SetComponent(entity, body);
        }
        
        for (core::Entity entity = 0; entity < entityManager_.GetEntitiesSize(); entity++)
        {
            if (!entityManager_.HasComponent(entity,
                static_cast<core::EntityMask>(core::ComponentType::BOXBODY2D)) ||
                entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::DESTROYED)))
                continue;
            for (core::Entity otherEntity = entity; otherEntity < entityManager_.GetEntitiesSize(); otherEntity++)
            {
                if (entity == otherEntity)
                    continue;
                if (!entityManager_.HasComponent(otherEntity,
                    static_cast<core::EntityMask>(core::ComponentType::BOXBODY2D)) ||
                    entityManager_.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::DESTROYED)))
                    continue;
                const BoxBody& boxbody1 = boxbodyManager_.GetComponent(entity);
                
                const BoxBody& boxbody2 = boxbodyManager_.GetComponent(otherEntity);
                
                if (boxbody1.bodyType == BodyType::STATIC && boxbody2.bodyType == BodyType::STATIC)
                    continue;

                if (Box2Box(
                    boxbody1.position.x - boxbody1.extends.x,
                    boxbody1.position.y - boxbody1.extends.y,
                    boxbody1.extends.x * 2.0f,
                    boxbody1.extends.y * 2.0f,
                    boxbody2.position.x - boxbody2.extends.x,
                    boxbody2.position.y - boxbody2.extends.y,
                    boxbody2.extends.x * 2.0f,
                    boxbody2.extends.y * 2.0f))
                {
                    core::LogDebug("Intersect");
                    onTriggerAction_.Execute(entity, otherEntity);
                }

            }
        }
    }

    void PhysicsManager::SetBoxBody(core::Entity entity, const BoxBody& body)
    {
        boxbodyManager_.SetComponent(entity, body);
    }

    const BoxBody& PhysicsManager::GetBody(core::Entity entity) const
    {
        return boxbodyManager_.GetComponent(entity);
    }

    void PhysicsManager::AddBoxBody(core::Entity entity)
    {
        boxbodyManager_.AddComponent(entity);
    }

    void PhysicsManager::RegisterTriggerListener(OnTriggerInterface& collisionInterface)
    {
        onTriggerAction_.RegisterCallback(
            [&collisionInterface](core::Entity entity1, core::Entity entity2) { collisionInterface.OnTrigger(entity1, entity2); });
    }

    void PhysicsManager::CopyAllComponents(const PhysicsManager& physicsManager)
    {
        boxbodyManager_.CopyAllComponents(physicsManager.boxbodyManager_.GetAllComponents());
    }
    
    /*bool isColliding(const BoxBody object, const BoxBody other)
    {
        float left = other.x - (object.x + object.width);
        float top = (other.y + other.height) - object.y;
        float right = (other.x + other.width) - object.x;
        float bottom = other.y - (object.y + object.height);

        return !(left > 0 || right < 0 || top < 0 || bottom > 0);
    }*/
}
