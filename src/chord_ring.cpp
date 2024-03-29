#include "chord_node.h"
#include <array>

static int PORT_BASE = 12345;

int main() {
    boost::asio::io_context io_context;
    // init servers (5)
    std::array<ChordNode, ChordNode::RING_SIZE> nodes = {
        ChordNode(io_context, 0, PORT_BASE),
        ChordNode(io_context, 1, PORT_BASE + 1),
        ChordNode(io_context, 2, PORT_BASE + 2),
        ChordNode(io_context, 3, PORT_BASE + 3),
        ChordNode(io_context, 4, PORT_BASE + 4),
        ChordNode(io_context, 5, PORT_BASE + 5),
        ChordNode(io_context, 6, PORT_BASE + 6),
        ChordNode(io_context, 7, PORT_BASE + 7),
    };

    // set up finger tables
    for (auto& node: nodes) {
        node.set_finger_table(nodes);

        std::vector<std::pair<int, std::string>> finger_table_addresses;
        for (int i == 0; i < ChordNode::M; i++) {
            int node_id = node.finger_table[i];
            finger_table_addresses.emplace_back(node_id, "localhost");
        }
    }

    // start servers
    for (auto& node: nodes) {
        node.start();
        std::cout << "Started server " << node.get_id() << " on port " << PORT_BASE + node.get_id() << std::endl; 
    }

    io_context.run();
    return 0;
}