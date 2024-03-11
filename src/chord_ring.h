#ifndef CHORD_RING_H
#define CHORD_RING_H

#include <vector>
#include <iostream>
#include "node.h"

class ChordRing {
private:
    std::vector<std::shared_ptr<Node>> nodes;
public:
    ChordRing();
    void add_node(const Node& node);
    void print_ring();
};

#endif /* CHORD_RING_H */