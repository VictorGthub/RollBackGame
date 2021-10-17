#include <graphics/sprite.h>
#include <engine/transform.h>

namespace core
{
    void SpriteManager::SetOrigin(Entity entity, sf::Vector2f origin)
    {
        components_[entity].setOrigin(origin);
    }

    void SpriteManager::SetTexture(Entity entity, const sf::Texture& texture)
    {
        components_[entity].setTexture(texture);
    }

    void SpriteManager::Draw(sf::RenderTarget& window)
    {
        for (Entity entity = 0; entity < components_.size(); entity++)
        {
            if (entityManager_.HasComponent(entity, static_cast<Component>(ComponentType::SPRITE)))
            {
                if (entityManager_.HasComponent(entity, static_cast<Component>(ComponentType::POSITION)))
                {
                    const auto position = transformManager_.GetPosition(entity);
                    components_[entity].setPosition(
                        position.x * pixelPerMeter + center_.x,
                        windowSize_.y - (position.y * pixelPerMeter + center_.y));
                }
                if(entityManager_.HasComponent(entity, static_cast<Component>(ComponentType::SCALE)))
                {
                    const auto scale = transformManager_.GetScale(entity);
                    components_[entity].setScale(scale.x, scale.y);
                }
                if (entityManager_.HasComponent(entity, static_cast<Component>(ComponentType::ROTATION)))
                {
                    const auto rotation = transformManager_.GetRotation(entity);
                    components_[entity].setRotation(rotation.value());
                }
                window.draw(components_[entity]);
            }
        }
    }

    void SpriteManager::SetColor(Entity entity, sf::Color color)
    {
        components_[entity].setColor(color);
    }
} // namespace core
