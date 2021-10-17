#include <string>

#include "network/network_server.h"

int main(int argc, char** argv)
{
    unsigned short port = 0;
    if (argc == 2)
    {
        std::string portArg = argv[1];
        port = std::stoi(portArg);
    }
    game::ServerNetworkManager server;
    if (port != 0)
    {
        server.SetTcpPort(port);
    }
    server.Init();
    sf::Clock clock;
    while (server.IsOpen())
    {
        const auto dt = clock.restart();
        server.Update(dt);
    }
    return 0;
}
