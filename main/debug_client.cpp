#include <engine/engine.h>

#include "network/client_debug_app.h"

int main(int argc, char** argv)
{
    core::Engine engine;
    game::NetworkDebugApp app;
    engine.RegisterSystem(&app);
    engine.RegisterDraw(&app);
    engine.RegisterDrawImGui(&app);
    engine.RegisterOnEvent(&app);

    engine.Run();

    return EXIT_SUCCESS;
}
