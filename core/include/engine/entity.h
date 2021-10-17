#pragma once

#include <cstdint>
#include <vector>
#include <limits>

namespace core
{
    
using Entity = std::uint32_t;
using EntityMask = std::uint32_t;

/**
 * \brief Manages the entities in an array using bitwise operations to know if it has components.
 */
class EntityManager
{
public:
    EntityManager();
    EntityManager(std::size_t reservedSize);

    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    // Normally called by ComponentManager
    void AddComponent(Entity entity, EntityMask mask);
    // Normally called by ComponentManager
    void RemoveComponent(Entity entity, EntityMask mask);
    [[nodiscard]] bool HasComponent(Entity entity, EntityMask mask) const;
    [[nodiscard]] bool EntityExists(Entity entity) const;

    [[nodiscard]] std::size_t GetEntitiesSize() const;

    static constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();
    static constexpr EntityMask INVALID_ENTITY_MASK = 0u;
private:
    std::vector<EntityMask> entityMasks_;
};

} // namespace core
