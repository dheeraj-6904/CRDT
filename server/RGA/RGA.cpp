#include "RGA.h"
#include<fstream>
#include <iostream>
RGA::RGA(std::string filename) {
    // Initialize the RGA with a head node
    this->filename = filename;  // Store the filename 
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

// This is a bonus functionality, not required for the project so far
void RGA::merge(const RGA& otherRGA) {
    // Merge another RGA instance into the current one
    for (const auto& node : otherRGA.nodes) {
        // Check if node is already in the current RGA
        auto nodeIndex = findNodeIndex(node.id);
        
        if (!nodeIndex.has_value()) {
            // Insert node if it doesn't exist
            nodes.push_back(node);
        } else if (node.isDeleted) {
            // Update deletion status if needed
            nodes[nodeIndex.value()].isDeleted = node.isDeleted;
        }
    }
}
// Return the current state of the RGA as a string(will bw used in saving of files and later merging)
std::string RGA::getState() const {
    std::string result;
    for (const auto& node : nodes) {
        if (!node.isDeleted) {
            result += node.value;
        }
    }
    return result;
}

// This function creates a new RGA instance from a file
void RGA::createRGAFromFile() {
    // SET the filepath for the RGA instance
    std::string filepath = "shared_folder/" + filename; //eg., shared_folder/file1.txt
    std::ifstream file(filepath); 
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::string line;
    std::string prevId = "head";
    while (std::getline(file, line)) {
        for (char ch : line) {
            std::string id = std::to_string(nodes.size() + 1);
            insert(id, prevId, std::string(1, ch));
            prevId = id;
        }
    }
    std::cout<<"RGA created from file: " << filename << std::endl;
}

// This function writes the current state of the RGA to a file
bool RGA::writeToFile() {
    // SET the filepath for the RGA instance
    std::string filepath = "shared_folder/" + filename; //eg., shared_folder/file1.txt
    try {
        std::ofstream file(filepath); // this filename is set in the createRGAFromFile function
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        std::string state = getState();     
        file << state;
        file.close();
    } 
    catch (const std::exception& e) {
        std::cerr << "Error writing to file: " << e.what() << std::endl;
        return false;
    }
    return true; // Return true if writing to file was successful
}

std::optional<size_t> RGA::findNodeIndex(const std::string& id) const {
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].id == id) {
            return i;
        }
    }
    return std::nullopt;
}
