#pragma once
#include <SFML/Graphics/VertexArray.hpp>

#include "graphics/graphics.h"

namespace sf
{
class RenderTarget;
}

namespace game
{

class StarBackground : public core::DrawInterface
{
public:
    void Init();
    void Draw(sf::RenderTarget& window) override;
private:
    static constexpr std::size_t starCount = 1024;
    sf::VertexArray vertexArray_;
};

}
