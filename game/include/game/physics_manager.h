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
    struct Body
    {
        core::Vec2f position = core::Vec2f::zero();
        core::Vec2f velocity = core::Vec2f::zero();
        core::degree_t angularVelocity = core::degree_t(0.0f);
        core::degree_t rotation = core::degree_t(0.0f);
        BodyType bodyType = BodyType::DYNAMIC;
    };

    struct Box
    {
        core::Vec2f extends = core::Vec2f::one();
        bool isTrigger = false;
    };

    class OnTriggerInterface
    {
    public:
        virtual ~OnTriggerInterface() = default;
        virtual void OnTrigger(core::Entity entity1, core::Entity entity2) = 0;
    };

    class BodyManager : public core::ComponentManager<Body, static_cast<core::EntityMask>(core::ComponentType::BODY2D)>
    {
    public:
        using ComponentManager::ComponentManager;
    };
    class BoxManager : public core::ComponentManager<Box, static_cast<core::EntityMask>(core::ComponentType::BOX_COLLIDER2D)>
    {
    public:
        using ComponentManager::ComponentManager;
    };

    class PhysicsManager
    {
    public:
        explicit PhysicsManager(core::EntityManager& entityManager);
        void FixedUpdate(sf::Time dt);
        [[nodiscard]] const Body& GetBody(core::Entity entity) const;
        void SetBody(core::Entity entity, const Body& body);
        void AddBody(core::Entity entity);

        void AddBox(core::Entity entity);
        void SetBox(core::Entity entity, const Box& box);
        [[nodiscard]] const Box& GetBox(core::Entity entity) const;

        void RegisterTriggerListener(OnTriggerInterface& collisionInterface);
        void CopyAllComponents(const PhysicsManager& physicsManager);
    private:
        core::EntityManager& entityManager_;
        BodyManager bodyManager_;
        BoxManager boxManager_;
        core::Action<core::Entity, core::Entity> onTriggerAction_;
    };

}
