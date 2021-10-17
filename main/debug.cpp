#include <network/full_debug_app.h>

#include "engine/engine.h"


int main()
{
    core::Engine engine;
    game::SimulationDebugApp app;
    engine.RegisterSystem(&app);
    engine.RegisterOnEvent(&app);
    engine.RegisterDraw(&app);
    engine.RegisterDrawImGui(&app);

    engine.Run();
    return 0;
}