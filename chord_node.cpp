#include "chord_node.h"
#include "murmur_hash_3.h"
#include "tfidf.h"
#include <sstream>
#include <cmath>

const int M = 3; // number of bits for node ids
TFIDF vectorizer(1.0);

ChordNode::ChordNode(boost::asio::io_context& io_context, int id, short port, int ring_size)
    : id(id), ring_size(ring_size), acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {}

int ChordNode::get_id() const {
    return id;
}

void ChordNode::start() {
    accept_connection();
}

void ChordNode::set_finger_table(boost::asio::io_context& io_context, const std::vector<ChordNode>& nodes) {
    int dynamic_ring_size = nodes.size();
    for (int i = 0; i < M; i++) {
        int suc_id = (id + static_cast<int>(std::pow(2, i))) % dynamic_ring_size;

        for (const auto& node : nodes) {
            int cur_id = node.get_id();

            if (cur_id >= suc_id) {
                finger_table[i] = cur_id;
                break;
            }
        }
    }
    // Update socket connections
    std::vector<std::pair<int, std::string>> finger_table_addresses;
    for (int node_id : finger_table) {
        finger_table_addresses.emplace_back(node_id, "localhost");
    }
    connect_finger_table(io_context, finger_table_addresses);
}

void ChordNode::accept_connection() {
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            handle_request(std::move(socket));
        }
        accept_connection();
    });
}

void ChordNode::handle_request(tcp::socket socket) {
    try {
        boost::asio::streambuf buffer;
        boost::asio::read_until(socket, buffer, "\n");
        std::istream stream(&buffer);
        std::string command;
        std::getline(stream, command);

        std::istringstream iss(command);
        std::string action, key, value;
        iss >> action >> key;
        if (action == "SET" || action == "UPD") {
            std::getline(iss >> std::ws, value);  // read the rest as the value
        }

        int hash = MurmurHash3::hash_key(key, 0, ring_size);
        std::string res;

        int closest_node_index = -1;
        for (int i = finger_table.size() - 1; i >= 0; --i) {
            if (finger_table[i] < hash) {
                closest_node_index = i;
                break;
            }
        }

        if (closest_node_index == -1 || finger_table[closest_node_index] == id) {
            // handle request
            if (action == "SET") {
                data[key] = vectorizer.vectorize(value);
                res = "Success: Vector '" + key + "' stored in server " + std::to_string(id) + ".\n";
            } else if (action == "GET") {
                auto it = data.find(key);
                if (it != data.end()) {
                    res = "Vector '" + key + "': " + it->second + ", retrieved from server " + std::to_string(id) + ".\n";
                } else {
                    res = "Error: Vector '" + key + "' not found.\n";
                }
            } else if (action == "DEL") {
                auto it = data.find(key);
                if (it != data.end()) {
                    data.erase(it);
                    res = "Success: Vector '" + key + "' deleted in server " + std::to_string(id) + ".\n";
                } else {
                    res = "Error: Vector '" + key + "' not found.\n";
                }
            } else if (action == "UPD") {
                auto it = data.find(key);
                if (it != data.end()) {
                    data[key] = vectorizer.vectorize(value);
                    res = "Success: Vector '" + key + "' updated in server " + std::to_string(id) + ".\n";
                } else {
                    res = "Error: Vector '" + key + "' not found.\n";
                }
            } else {
                res = "Error: Command not recognized.\n";
            }
        } else {
            // forward request
            boost::asio::write(*finger_table_sockets[closest_node_index], boost::asio::buffer(command + "\n"));
            boost::asio::streambuf forward_response;
            boost::asio::read_until(*finger_table_sockets[closest_node_index], forward_response, "\n");
            std::istream forward_response_stream(&forward_response);
            std::getline(forward_response_stream, res);
        }

        // send response
        boost::asio::write(socket, boost::asio::buffer(res));
    } catch (std::exception& e) {
        std::cerr << "Error handling request: " << e.what() << "\n";
    }
}

void ChordNode::connect_finger_table(boost::asio::io_context& io_context, const std::vector<std::pair<int, std::string>>& finger_table_addresses) {
    finger_table_sockets.clear();
    for (const auto& [node_id, address] : finger_table_addresses) {
        auto socket = std::make_unique<tcp::socket>(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(*socket, resolver.resolve(address, std::to_string(PORT_BASE + node_id)));
        finger_table_sockets.push_back(std::move(socket));
    }
}

void ChordNode::join(boost::asio::io_context& io_context, std::vector<ChordNode>& nodes) {
    std::cout << "Node " << id << " joining the network." << std::endl;
    
    // update finger table
    set_finger_table(io_context, nodes);

    // update other nodes' finger tables to include this node
    for (auto& node : nodes) {
        if (node.get_id() != id) {
            node.update_finger_table(io_context, *this);
        }
    }
}

void ChordNode::leave(boost::asio::io_context& io_context, std::vector<ChordNode>& nodes) {
    std::cout << "Node " << id << " leaving the network." << std::endl;

    // remove this node from other nodes' finger tables
    for (auto& node : nodes) {
        if (node.get_id() != id) {
            node.remove_from_finger_table(io_context, id, nodes);
        }
    }
}

void ChordNode::update_finger_table(boost::asio::io_context& io_context, const ChordNode& new_node) {
    for (int i = 0; i < finger_table.size(); i++) {
        int expected_id = (id + static_cast<int>(std::pow(2, i))) % ring_size;
        if (new_node.get_id() >= expected_id && (finger_table[i] == -1 || new_node.get_id() < finger_table[i])) {
            finger_table[i] = new_node.get_id();
            // update socket connection
            connect_finger_table_sockets(io_context, i, new_node.get_id()); 
        }
    }
}

void ChordNode::remove_from_finger_table(boost::asio::io_context& io_context, int node_id, const std::vector<ChordNode>& nodes) {
    for (int i = 0; i < finger_table.size(); i++) {
        if (finger_table[i] == node_id) {
            // find next suitable node
            int target_id = (id + static_cast<int>(std::pow(2, i))) % ring_size;
            finger_table[i] = find_next_node_id(target_id, nodes);
            // update socket connection
            connect_finger_table_sockets(io_context, i, finger_table[i]); 
        }
    }
}

int ChordNode::find_next_node_id(int target_id, const std::vector<ChordNode>& nodes) {
    for (const auto& node : nodes) {
        if (node.get_id() >= target_id) {
            return node.get_id();
        }
    }
    // if no node has an id greater than or equal to target_id, return the first node in the ring
    return !nodes.empty() ? nodes.front().get_id() : -1;
}

void ChordNode::connect_finger_table_sockets(boost::asio::io_context& io_context, int index, int node_id) {
    finger_table_sockets[index] = std::make_unique<tcp::socket>(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(*finger_table_sockets[index], resolver.resolve("localhost", std::to_string(PORT_BASE + node_id)));
}