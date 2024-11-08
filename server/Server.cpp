#include "Server.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/asio.hpp>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "RGA/RGA.h"
// Constructor
Server::Server(const std::string& sharedFolder, const std::string& ipListFile, int port)
    : sharedFolder(sharedFolder) {
    // load authorized IPs from file and RGA files from shared folder
    loadAuthorizedIPs(ipListFile);
    loadRGAFilesFromFolder();
    acceptor = new boost::asio::ip::tcp::acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
}

// Destructor
Server::~Server() {
    delete acceptor;
}

// function to load files from shared folder to rga instances
void Server::loadRGAFilesFromFolder() {
    try {
        std::filesystem::path folderPath(sharedFolder);
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                rgaFiles[filename] = RGA(filename);
                rgaFiles[filename].createRGAFromFile();
            }
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "Error loading RGA files from folder: " << e.what() << std::endl;
    }
}
void Server::loadAuthorizedIPs(const std::string& ipListFile) {
    std::ifstream ipFile(ipListFile);
    std::string ip;
    while (std::getline(ipFile, ip)) {
        authorizedIPs.push_back(ip);
    }
    ipFile.close();
}

// Check if the client IP is authorized
bool Server::isAuthorized(const std::string& clientIP) {
    //if (clientIP == "172.31.1.48") return true;  // hardcoded for testing purpose
    for (const auto& ip : authorizedIPs) {
        if (clientIP == ip) { 
            return true;
        }
    }
    return false;
}

// Start the server
void Server::start() {
    // Display your ip and port 
    boost::asio::ip::tcp::endpoint endpoint = acceptor->local_endpoint();
    std::string serverIP = endpoint.address().to_string();
    int serverPort = endpoint.port();

    // Print the server IP and port
    std::cout << "Server has been started and is running at IP: " 
              << serverIP << " on port: " << serverPort << std::endl;

    while (true) {
        boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(io_service);
        acceptor->accept(*socket);

        std::string clientIP = socket->remote_endpoint().address().to_string();

        if (!isAuthorized(clientIP)) {
            std::cerr << "Unauthorized access attempt from " << clientIP << std::endl;
            delete socket;
            continue;
        }

        std::cout << "Authorized connection from " << clientIP << std::endl;
        // Spawn a new thread for each client
        std::thread(&Server::handleClient, this, socket).detach();
    }
}

// Handle client requests
void Server::handleClient(boost::asio::ip::tcp::socket* socket) {
    try {
        while (true) {
            char request[1024];
            std::size_t len = socket->read_some(boost::asio::buffer(request));
            std::string requestStr(request, len);

            std::istringstream iss(requestStr);
            std::string command1, command2, command3,command4,content;
            iss >> command1 >> command2 >> command3 >> command4;

            // Display the request
            std::cout << "Request: " << requestStr << " from: " << socket->remote_endpoint().address().to_string() << "\n";

            if (command1 == "GET") {   
                sendFileContent(socket, command2);
            }
            else if (command1 == "SHARED") {    
                SendSharedFolder(socket);
            } 

            else if (command1 == "DELETE") {    
                Delete_File(socket,command2);
            }
            else if (command1 == "SAVE") { 
                std::cout<<"saving file " << command2<<std::endl;   
                Save_File(socket,command2); //cmd2 contain filename
            }
            else if (command1 == "CREATE") {    
                Create_File(socket,command2); //cmd2 contain new file name
            }

            else if (command1 == "QUIT") {
                std::cout << "Client " << socket->remote_endpoint().address().to_string() << " disconnected." << std::endl;
                break;  // Exit loop, close socket
            }
            else if (command1 == "INSERT") {
                std::string id = std::to_string(rgaFiles[command2].nodes_size);
                std::string prevID = rgaFiles[command2].getIthNode(std::stoi(command4));
                rgaFiles[command2].insert(id,prevID,command3);
            }

        }
    } catch (...) {
        std::cerr << "Error handling client request." << std::endl;
    }

    delete socket;  // Clean up socket after client disconnects
}


// Function to get list of files in shared folder
std::string listFilesInFolder(const std::string& folderPath) {
    std::string filesString;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                filesString += entry.path().filename().string() + "\n";
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error (in listFilesInFolder function): " << e.what() << std::endl;
    }

    return filesString;
}

// To send shared folder content
void Server::SendSharedFolder(boost::asio::ip::tcp::socket* socket) {
    std::string content = listFilesInFolder(sharedFolder);
    boost::asio::write(*socket, boost::asio::buffer(content));
}

// Function to delete a file
void Server::Delete_File(boost::asio::ip::tcp::socket* socket, std::string filename) {
    std::string fullPath = sharedFolder + "/" + filename;
    try {
        if (std::filesystem::exists(fullPath)) {
            std::filesystem::remove(fullPath);  
            std::string response = "100"; // 100 means suscess
            boost::asio::write(*socket, boost::asio::buffer(response));
            std::cout << "File " << filename << " deleted from shared folder.\n";

            // Remove the RGA instance from the map
            rgaFiles.erase(filename);  

        } else {
            std::string response = "Error: File " + filename + " not found.\n";
            boost::asio::write(*socket, boost::asio::buffer(response));  
            std::cerr << "Error: File " << filename << " not found in shared folder.\n";
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::string response = "Error deleting file: " + std::string(e.what()) + "\n";
        boost::asio::write(*socket, boost::asio::buffer(response));  // Notify the client of the error
        std::cerr << "Error deleting file " << filename << ": " << e.what() << std::endl;
    }
}

// Function to create a new file
void Server::Create_File(boost::asio::ip::tcp::socket* socket, std::string filename) {
    std::string fullPath = sharedFolder + "/" + filename;
    try {
        // if the file already exixt
        if (std::filesystem::exists(fullPath)) { 
            std::string response = "file already exixt !"; 
            boost::asio::write(*socket, boost::asio::buffer(response));
            std::cout << " requested file already exixst"<<std::endl;
        } else {
            std::ofstream file(fullPath);
            if(file){
                std::string response = filename +" created sucessfullly !"; 
                boost::asio::write(*socket, boost::asio::buffer(response));

                // also create the rga to to the file
                rgaFiles[filename] = RGA(filename);  // Create a new RGA instance for the file
                std::cout << "File " << filename << " created in shared folder."<<std::endl;
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::string response = "Error creating new file!! " + std::string(e.what());
        boost::asio::write(*socket, boost::asio::buffer(response));  // Notify the client of the error
        std::cerr << "Error creating new file!! " << filename << ": " << e.what() << std::endl;
    }
}

// Function to Save a file
void Server::Save_File(boost::asio::ip::tcp::socket* socket, std::string filename) {
    // check if the file exist in rgafiles
    if (rgaFiles.find(filename) == rgaFiles.end()) {
        std::cout<<"check";
        std::string response = "Error: File " + filename + " not found in RGA instances.\n";
        boost::asio::write(*socket, boost::asio::buffer(response));
        std::cerr << "Error: File " << filename << " not found in RGA instances.\n";
        return;
    }
    // get the respective rga and write to file

    rgaFiles[filename].writeToFile();
    std::cout<<"check";
}



// Send file content to the client
void Server::sendFileContent(boost::asio::ip::tcp::socket* socket, const std::string& filename) {
    if (rgaFiles.find(filename) == rgaFiles.end()) {
        std::cerr << " cant find the rga of the file: " << filename << std::endl;
        return;
    }
    std::string filecontent = rgaFiles[filename].getState();  // Get the file content from the RGA
    boost::asio::write(*socket, boost::asio::buffer(filecontent));
    std::cout << filename << " data sent" << std::endl;
}

// // Handle file changes from clients
// void Server::handleFileChange(const std::string& filename, const std::string& content) {
//     std::string fullPath = sharedFolder + "/" + filename;

//     std::ofstream file(fullPath, std::ios::app);
//     if (file.is_open()) {
//         file << content;
//         file.close();
//     } else {
//         std::cerr << "Failed to write to file: " << filename << std::endl;
//     }
// }

// Broadcast changes to other clients
void Server::broadcastChange(const std::string& filename, const std::string& content, const std::string& senderIP) {
    for (const auto& ip : authorizedIPs) {
        if (ip == senderIP) continue;  // Skip sender

        try {
            boost::asio::ip::tcp::socket socket(io_service);
            boost::asio::ip::tcp::resolver resolver(io_service);
            boost::asio::ip::tcp::resolver::query query(ip, "12345");
            boost::asio::connect(socket, resolver.resolve(query));

            std::string message = "UPDATE " + filename + " " + content;
            boost::asio::write(socket, boost::asio::buffer(message));
        } catch (...) {
            std::cerr << "Error broadcasting to client " << ip << std::endl;
        }
    }
}

// Main function to start the server
int main(int argc, char* argv[]) {
    std::string sharedFolder = "shared_folder";
    std::string ipListFile = "collaborators_ip.txt";
    int port = 8080;

    try {
        Server server(sharedFolder, ipListFile, port);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
