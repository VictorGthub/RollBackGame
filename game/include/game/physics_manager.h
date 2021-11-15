#pragma once
#include "game_globals.h"
#include "engine/component.h"
#include "engine/entity.h"
#include "maths/angle.h"
#include "maths/vec2.h"

#include <SFML/System/Time.hpp>

#include "utils/action_utility.h"

namespace game
{
    enum class BodyType
    {
        DYNAMIC,
        STATIC
    };
    struct BoxBody
    {
        core::Vec2f position = core::Vec2f::zero();
        core::Vec2f velocity = core::Vec2f::zero();
        core::degree_t angularVelocity = core::degree_t(0.0f);
        core::degree_t rotation = core::degree_t(0.0f);
        BodyType bodyType = BodyType::DYNAMIC;
        core::Vec2f extends;
       
        bool isTrigger = false;
        
    };

   

    class OnTriggerInterface
    {
    public:
        virtual ~OnTriggerInterface() = default;
        virtual void OnTrigger(core::Entity entity1, core::Entity entity2) = 0;
    };

    class BoxBodyManager : public core::ComponentManager<BoxBody, static_cast<core::EntityMask>(core::ComponentType::BOXBODY2D)>
    {
    public:
        using ComponentManager::ComponentManager;
    };
    
    class PhysicsManager
    {
    public:
        explicit PhysicsManager(core::EntityManager& entityManager);
        void FixedUpdate(sf::Time dt);
        [[nodiscard]] const BoxBody& GetBody(core::Entity entity) const;
        void SetBoxBody(core::Entity entity, const BoxBody& body);
        void AddBoxBody(core::Entity entity);
        
        void RegisterTriggerListener(OnTriggerInterface& collisionInterface);
        void CopyAllComponents(const PhysicsManager& physicsManager);
        void ResolveCollision(BoxBody& boxbody1, BoxBody& boxbody2);
    private:
        core::EntityManager& entityManager_;
        BoxBodyManager boxbodyManager_;
        core::Action<core::Entity, core::Entity> onTriggerAction_;
    };

}
