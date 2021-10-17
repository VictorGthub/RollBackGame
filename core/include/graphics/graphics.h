#pragma once

namespace sf
{
class RenderWindow;
}

namespace core
{
class DrawInterface
{
public:
    virtual ~DrawInterface() = default;
    virtual void Draw(sf::RenderTarget& window) = 0;
};

class DrawImGuiInterface
{
public:
    virtual ~DrawImGuiInterface() = default;
    virtual void DrawImGui() = 0;
};
}
