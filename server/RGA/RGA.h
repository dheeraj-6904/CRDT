#ifndef RGA_H
#define RGA_H

#include <map>
#include <vector>
#include <string>
#include <optional>

//Structure representing a node in the RGA
struct RGANode {
    std::string value;        // The actual character or say elementy
    std::string id;             // Unique identifier
    std::string prevId;        // ID of the previous node
    bool isDeleted;         // Marker for logical deletion

    RGANode(std::string v, std::string id, std::string prevId, bool del = false)
        : value(v), id(id), prevId(prevId), isDeleted(del) {}
};

// RGA class for managing the array
class RGA {
public:
    RGA() = default;  // Default constructor
    RGA(std::string filename);  // Constructor
    int nodes_size;

    // Insert a character after a specific node
    void insert(const std::string& id, const std::string& prevId, const std::string& value);

    // Delete a character by its unique ID
    void erase(const std::string& id);

    // Merge two RGAs (for syncing between replicas)
    void merge(const RGA& otherRGA);

    // Get the current state of the array as a string
    std::string getState() const;

    // Function to generate RGA structure from a file 
    void createRGAFromFile();

    // Function to save the RGA structure to a file
    bool writeToFile();

    //
    std::string getIthNode(int i);
    

private:
    // Internal function to find node index by its ID
    std::optional<size_t> findNodeIndex(const std::string& id) const;

    std::vector<RGANode> nodes;  // List of nodes representing the text array
    std::string filename;
};


#endif // RGA_H
