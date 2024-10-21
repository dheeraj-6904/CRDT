#include "RGA.h"
#include<fstream>
#include <iostream>
RGA::RGA() {
    // Initialize the RGA with a head node 
    nodes.push_back(RGANode("", "head", ""));  // Head node with empty value
}

void RGA::insert(const std::string& id, const std::string& prevId, const std::string& value) {
    // Find the index of the node where the new node will be inserted after
    auto prevIndex = findNodeIndex(prevId);
    
    // If prevId is found, insert the new node after it
    if (prevIndex.has_value()) {
        nodes.insert(nodes.begin() + prevIndex.value() + 1, RGANode(value, id, prevId));
    }
}

void RGA::erase(const std::string& id) {
    // Find the node by its unique ID
    auto nodeIndex = findNodeIndex(id);
    
    // Mark the node as deleted (logical deletion)
    if (nodeIndex.has_value()) {
        nodes[nodeIndex.value()].isDeleted = true;
    }
}



std::optional<size_t> RGA::findNodeIndex(const std::string& id) const {
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].id == id) {
            return i;
        }
    }
    return std::nullopt;
}
