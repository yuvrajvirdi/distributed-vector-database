#include "chord_node.h"
#include <iostream>
#include <array>

static int PORT_BASE = 12345;

int main() {
    boost::asio::io_context io_context;
    // init servers (4)
    std::vector<ChordNode> nodes = {
        ChordNode(io_context, 0, PORT_BASE),
        ChordNode(io_context, 1, PORT_BASE + 1),
        ChordNode(io_context, 2, PORT_BASE + 2),
        ChordNode(io_context, 3, PORT_BASE + 3),
    };

    // start servers
    for (auto& node: nodes) {
        node.start();
        std::cout << "Started server " << node.get_id() << " on port " << PORT_BASE + node.get_id() << std::endl; 
    }

    io_context.run();
    return 0;
}