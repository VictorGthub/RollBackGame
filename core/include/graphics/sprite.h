#pragma once

#include "engine/component.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "graphics.h"

namespace core
{
    class TransformManager;

    /**
     * \brief Manages sprites, order by greater entity index, background entity < foreground entity
     * Positions are centered at the center of the render target and use pixelPerMeter from globals.h
     */
    class SpriteManager :
        public ComponentManager<sf::Sprite, static_cast<Component>(ComponentType::SPRITE)>,
        public DrawInterface
    {
    public:
        SpriteManager(EntityManager& entityManager, TransformManager& transformManager) :
            ComponentManager(entityManager),
            transformManager_(transformManager)
        {

        }
        void SetOrigin(Entity entity, sf::Vector2f origin);
        void SetTexture(Entity entity, const sf::Texture& texture);
        void SetCenter(sf::Vector2f center) { center_ = center; }
        void SetWindowSize(sf::Vector2f windowSize) { windowSize_ = windowSize; }
        void Draw(sf::RenderTarget& window) override;
        void SetColor(Entity entity, sf::Color color);

    protected:
        TransformManager& transformManager_;
        sf::Vector2f center_{};
        sf::Vector2f windowSize_{};

    };

}
