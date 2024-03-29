#ifndef CHORD_NODE_H
#define CHORD_NODE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <array>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class ChordNode {
public:
    static const int M = 3;  // num bits for node IDs
    static const int RING_SIZE = 1 << M; // size of the ring

    ChordNode(boost::asio::io_context& io_context, int id, short port);

    int get_id() const;
    void start();
    void set_finger_table(const std::array<ChordNode, RING_SIZE>& nodes);
    void connect_finger_table(boost::asio::io_context& io_context, const std::vector<std::pair<int, std::string>>& finger_table_addresses);

private:
    void accept_connection();
    void handle_request(tcp::socket socket);
    int hash_key(const std::string& key);

    int id;
    tcp::acceptor acceptor;
    std::unordered_map<std::string, std::string> data;
    std::array<int, M> finger_table;
    std::vector<std::unique_ptr<tcp::socket>> finger_table_sockets;
};

#endif // CHORD_NODE_H