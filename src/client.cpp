#include "client.h"
#include <cmath>
#include <sstream>

std::string extract_key(const std::string& command) {
    std::istringstream iss(command);
    std::string action, key;
    iss >> action >> key;
    return key;
}

int hash_key(const std::string& key) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % 8;
    }
    return hash;
}

int determine_server(int hash) {
    return 12345 + hash;
}

Client::Client(boost::asio::io_context& io_context, const std::string& host, const std::string& port)
    : socket(io_context) {
    tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve(host, port));
}

std::string Client::send_command(const std::string& command) {
    std::string key = extract_key(command);
    int hash = hash_key(key);
    int server_port = determine_server(hash);

    // connect to proper server
    tcp::resolver resolver(socket.get_executor().context());
    boost::asio::connect(socket, resolver.resolve("localhost", std::to_string(server_port)));

    // forward request
    boost::asio::write(socket, boost::asio::buffer(command + "\n"));
    boost::asio::streambuf res;
    boost::asio::read_until(socket, res, "\n");
    std::istream response_stream(&res);
    std::string res_str;
    std::getline(response_stream, res_str);

    // graceful close
    socket.close();

    return res_str;
}
