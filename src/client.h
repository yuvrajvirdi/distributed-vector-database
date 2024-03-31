#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Client {
public:
    Client(const std::string& host, const std::string& port);
    ~Client();

    std::string send_command(const std::string& command);

private:
    boost::asio::io_context io_context;
    tcp::socket socket;
};

#endif