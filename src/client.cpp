#include "client.h"
#include <sstream>

Client::Client(const std::string& host, const std::string& port)
    : socket(io_context) {
    tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve(host, port));
}

Client::~Client() {
    socket.close();
}

std::string Client::send_command(const std::string& command) {
    boost::asio::write(socket, boost::asio::buffer(command + "\n"));
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\n");
    std::istream response_stream(&response);
    std::string response_str;
    std::getline(response_stream, response_str);
    return response_str;
}

