#include "client.h"
#include <iostream>

int main() {
    try {
        boost::asio::io_context io_context;
        Client client(io_context);

        std::string res = client.send_command("SET vector1 \"this is a sample sentence\"");
        std::cout << res << std::endl;

        res = client.send_command("GET vector1");
        std::cout << res << std::endl;

        res = client.send_command("DEL vector1");
        std::cout << res << std::endl;

        res = client.send_command("UPD vector1 \"this is an updated sentence\"");
        std::cout << res << std::endl;


    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
