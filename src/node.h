#ifndef NODE_H
#define NODE_H

#include <string>
#include <memory>
#include <vector>

struct Node;

// finger table entry
struct Finger {
    std::shared_ptr<Node> node; // pointer to node in the finer table
};

struct Node {
    std::string id;                     // id of the node
    std::shared_ptr<Node> succ;         // successor node
    std::shared_ptr<Node> pred;         // predecessor node
    std::vector<Finger> finger_table;   // finger table for efficient lookups

    Node(const std::string& id, int m); // constructor with id and size
};

#endif // NODE_H