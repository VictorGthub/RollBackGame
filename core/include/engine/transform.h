#pragma once
#include "component.h"
#include "maths/angle.h"
#include "maths/vec2.h"

#include <engine/entity.h>

namespace core
{
/**
 * \brief Position are in physics space (using pixelPerMeter in globals.h)
 */
class PositionManager : public ComponentManager<Vec2f, static_cast<Component>(ComponentType::POSITION)>
{
public:
    using ComponentManager::ComponentManager;
};

class ScaleManager : public ComponentManager<Vec2f, static_cast<std::uint32_t>(ComponentType::SCALE)>
{
public:
    using ComponentManager::ComponentManager;
    void AddComponent(Entity entity) override;
};

class RotationManager : public ComponentManager<degree_t, static_cast<std::uint32_t>(ComponentType::ROTATION)>
{
public:
    using ComponentManager::ComponentManager;
};

class TransformManager
{
public:
    TransformManager(EntityManager& entityManager);

    [[nodiscard]] Vec2f GetPosition(Entity entity) const;
    [[nodiscard]] const std::vector<Vec2f>& GetAllPositions() const;
    void SetPosition(Entity entity, Vec2f position);

    [[nodiscard]] Vec2f GetScale(Entity entity) const;
    [[nodiscard]] const std::vector<Vec2f>& GetAllScales() const;
    void SetScale(Entity entity, Vec2f scale);

    [[nodiscard]] degree_t GetRotation(Entity entity) const;
    [[nodiscard]] const std::vector<degree_t>& GetAllRotations() const;
    void SetRotation(Entity entity, degree_t rotation);

    void AddComponent(Entity entity);
    void RemoveComponent(Entity entity);
    
private:
    PositionManager positionManager_;
    ScaleManager scaleManager_;
    RotationManager rotationManager_;
};

}
