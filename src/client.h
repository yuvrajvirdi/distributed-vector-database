#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io_context, const std::string& host, const std::string& port);

    std::string send_command(const std::string& command);

private:
    tcp::socket socket;
};

#endif // CLIENT_H