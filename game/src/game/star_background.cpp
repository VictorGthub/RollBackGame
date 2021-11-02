#include <game/star_background.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include "maths/basic.h"
#include <engine/globals.h>

namespace game
{
    void StarBackground::Init()
    {
        vertexArray_ = sf::VertexArray(sf::Points, starCount);
        /*
        vertexArray_[0].position = sf::Vector2f(0 * core::pixelPerMeter, 0 * core::pixelPerMeter);
        vertexArray_[1].position = sf::Vector2f(0 * core::pixelPerMeter, 10 * core::pixelPerMeter);
        */

        for (std::size_t i = 0; i < starCount; i++)
        {
            auto& vertex = vertexArray_[i];
            vertex.color = sf::Color::Red;
            vertex.position = sf::Vector2f(
                core::RandomRange(-50.0f, 50.0f) * core::pixelPerMeter,
                core::RandomRange(-50.0f, 50.0f) * core::pixelPerMeter);
        }
    }

    void StarBackground::Draw(sf::RenderTarget& window)
    {
        window.draw(vertexArray_);
    }
}
