#pragma once

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>

namespace core
{
class OnEventInterface;
class SystemInterface;
class DrawInterface;
class DrawImGuiInterface;

class Engine
{
public:
    void Run();

    void RegisterSystem(SystemInterface*);
    void RegisterOnEvent(OnEventInterface*);
    void RegisterDraw(DrawInterface*);
    void RegisterDrawImGui(DrawImGuiInterface*);
protected:
    void Init();
    void Update(sf::Time dt);
    void Destroy();

    std::vector<SystemInterface*> systems_;
    std::vector<OnEventInterface*> eventInterfaces_;
    std::vector<DrawInterface*> drawInterfaces_;
    std::vector<DrawImGuiInterface*> drawImGuiInterfaces_;
    std::unique_ptr<sf::RenderWindow> window_;
};

} // namespace core
