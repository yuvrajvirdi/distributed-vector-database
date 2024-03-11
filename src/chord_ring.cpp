#include "chord_ring.h"

ChordRing::ChordRing() {
   std::shared_ptr<Node> initial_node = std::make_shared<Node>("0", 8);
   initial_node->succ = initial_node;
   initial_node->pred = initial_node;
   nodes.push_back(initial_node);
}

void ChordRing::add_node(const std::string& id, int m) {
    std::shared_ptr<Node> new_node = std::make_shared<Node>(id, m);
    nodes.push_back(new_node);
}

