#include "Server.hpp"
#include <asio.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        short port = 9000;
        if (argc > 1) port = static_cast<short>(std::atoi(argv[1]));

        asio::io_context io_context;
        Server server(io_context, port);

        std::cout << "Server listening on port " << port << std::endl;
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}