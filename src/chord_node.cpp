#include "chord_node.h"
#include <sstream>
#include <cmath>

ChordNode::ChordNode(boost::asio::io_context& io_context, int id, short port)
    : id(id), acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {}

int ChordNode::get_id() const {
    return id;
}

void ChordNode::start() {
    accept_connection();
}

void ChordNode::set_finger_table(const std::array<ChordNode, RING_SIZE>& nodes) {
    for (int i = 0; i < M; i++) {
        int suc_id = (id + static_cast<int>(std::pow(2, i))) % RING_SIZE;

        for (const auto& node: nodes) {
            int cur_id = node.get_id();

            if (cur_id >= suc_id) {
                finger_table[i] = cur_id;
                break;
            }
        }
    }
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

        int hash = hash_key(key);
        std::string res;

        // find predecessor node of the key
        int pred = -1;
        for (int i = M - 1; i >= 0; i--) {
            if (finger_table[i] < hash) {
                pred = i;
                break;
            }
        }

        if (pred == -1) {
            if (action == "SET") {
                data[key] = value;
                res = "Success: Vector '" + key + "' stored in server " + std::to_string(id) + ".\n";
            } else if (action == "GET") {
                auto it = data.find(key);
                if (it != data.end()) {
                    res = "Vector '" + key + "': " + it->second + ", retrieved from server" + std::to_string(id) + ".\n";
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
                    data[key] = value;
                    res = "Success: Vector '" + key + "' updated in server " + std::to_string(id) + ".\n";
                } else {
                    res = "Error: Vector '" + key + "' not found.\n";
                }
            } else {
                res = "Error: Command not recognized.\n";
            }
        } else {
            boost::asio::write(*finger_table_sockets[pred], boost::asio::buffer(command + "\n"));
            boost::asio::streambuf fwd_res;
            boost::asio::read_until(*finger_table_sockets[pred], fwd_res, "\n");
            std::istream fwd_res_stream(&fwd_res);
            std::getline(fwd_res_stream, res);
        }
    boost::asio::write(socket, boost::asio::buffer(res));

    } catch (std::exception& e) {
        std::cerr << "error handling request: " << e.what() << "\n";
    }
}

int ChordNode::hash_key(const std::string& key) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % RING_SIZE;
    }
    return hash;
}

void ChordNode::connect_finger_table(boost::asio::io_context& io_context, const std::vector<std::pair<int, std::string>>& finger_table_addresses) {
    finger_table_sockets.clear();
    for (const auto& [id, address] : finger_table_addresses) {
        auto socket = std::make_unique<tcp::socket>(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(*socket, resolver.resolve(address, std::to_string(12345 + id)));
        finger_table_sockets.push_back(std::move(socket));
    }
}