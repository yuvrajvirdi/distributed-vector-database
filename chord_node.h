#ifndef CHORD_NODE_H
#define CHORD_NODE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class ChordNode {
public:
    ChordNode(boost::asio::io_context& io_context, int id, short port, int ring_size);

    static const int PORT_BASE = 12345;
    int get_id() const;
    void start();
    void set_finger_table(boost::asio::io_context& io_context, const std::vector<ChordNode>& nodes);
    void connect_finger_table(boost::asio::io_context& io_context, const std::vector<std::pair<int, std::string>>& finger_table_addresses);
    void join(boost::asio::io_context& io_context, std::vector<ChordNode>& nodes);
    void leave(boost::asio::io_context& io_context, std::vector<ChordNode>& nodes);
    void update_finger_table(boost::asio::io_context& io_context, const ChordNode& newNode);
    void remove_from_finger_table(boost::asio::io_context& io_context, int nodeId, const std::vector<ChordNode>& nodes);

private:
    void accept_connection();
    void handle_request(tcp::socket socket);
    int find_next_node_id(int target_id, const std::vector<ChordNode>& nodes);
    void connect_finger_table_sockets(boost::asio::io_context& io_context, int index, int node_id);

    int id;
    int ring_size;
    tcp::acceptor acceptor;
    std::unordered_map<std::string, std::string> data;
    std::vector<int> finger_table;
    std::vector<std::unique_ptr<tcp::socket>> finger_table_sockets;
};

#endif 
