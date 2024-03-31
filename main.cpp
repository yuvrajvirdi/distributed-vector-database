#include "client.h"
#include <iostream>

int main() {
    try {
        Client client("localhost", "12345");

        std::string response = client.send_command("SET vector1 \"this is a sample sentence\"");
        std::cout << response << std::endl;

        response = client.send_command("GET vector1");
        std::cout << response << std::endl;

        response = client.send_command("DEL vector1");
        std::cout << response << std::endl;

        response = client.send_command("UPD vector1 \"this is an updated sentence\"");
        std::cout << response << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
