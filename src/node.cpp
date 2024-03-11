#include "node.h"

Node::Node(const std::string& id, int m) : id(id), succ(nullptr), pred(nullptr) {
    // init finger table with empty entries
    finger_table.resize(m);
}