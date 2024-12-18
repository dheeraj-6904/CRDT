#include "NetworkManager.h"
#include <boost/asio.hpp>
#include <iostream>

NetworkManager::NetworkManager() {
    // Initialize a socket connection
    socket = new boost::asio::ip::tcp::socket(io_service);
}

NetworkManager::~NetworkManager() {
    delete socket;
}

// connect to server using socket connection(IP + Port number)
bool NetworkManager::connectToServer(const std::string& ip, int port) {
    try {
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(ip, std::to_string(port));
        boost::asio::connect(*socket, resolver.resolve(query));
        std::cout << "Connected to server at " << ip << ":" << port << std::endl;
        return true;
    } catch (...) {
        std::cerr << "Error connecting to server!" << std::endl;
        return false;
    }
}

void NetworkManager::sendChangeToServer(const std::string& message) {
    try {
        boost::asio::write(*socket, boost::asio::buffer(message));
        
    } catch (...) {
        std::cerr << "Error sending data to server!" << std::endl;
    }
}

// for taking the files here in client from server
std::string NetworkManager::get_shared_folder() {
    try {
        std::string message = "SHARED NILL"; // 100 to get shared folder
        boost::asio::write(*socket, boost::asio::buffer(message));

        // recive the changes
        std::string data = receiveData();
        return data;
    } 

    catch (...) {
        std::cerr << "Error getting the shared folder forom the server!(connection problem)" << std::endl;
    }
}

// load the data of a specific file
std::string NetworkManager::load_file(std::string filename) {
    try {
        if (!socket->is_open()) {
            std::cerr << "Socket is closed!" << std::endl;
            std::cout<<"socket closed";
            return "";
        }

        std::string message = "GET " + filename; // to get file content
        std::cout<<"requesting : "<<message<< std::endl;

        boost::asio::write(*socket, boost::asio::buffer(message));

        // recive the changes
        std::string data = receiveData();
        std::cout<<"data received "<<std::endl;
        return data;  
    } 

    catch (...) {
        std::cerr << "Error sending data to server!" << std::endl;
    }
}

// General function for sending commands
std::string NetworkManager::send_command(std::string command1,std::string command2) {
    try {
        if (!socket->is_open()) {
            std::cerr << "Socket is closed!" << std::endl;
            std::cout<<"Socket closed";
            return "";
        }

        std::string message = command1  +" " + command2; // the command
        std::cout<<"Requesting : "<<message<< std::endl;

        // Write the message to the socket
        boost::asio::write(*socket, boost::asio::buffer(message));

        // Recive the changes
        std::string data = receiveData();
        std::cout<<"Data received "<<std::endl;
        return data;  
    } 

    catch (...) {
        std::cerr << "Error sending data to server!" << std::endl;
    }
}

// Function to receive any kind of data from server
std::string NetworkManager::receiveData() {
    char reply[1048];
    size_t reply_length = socket->read_some(boost::asio::buffer(reply));
    return std::string(reply, reply_length);
}
