#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <boost/asio.hpp>

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();
    
    bool connectToServer(const std::string& ip, int port);
    void sendChangeToServer(const std::string& operation);
    std::string receiveData();  // Receive data from server

    std::string get_shared_folder(); // to get the shared folder
    std::string load_file(std::string filename); // to get the file content

    std::string send_command(std::string command1,std::string command2);
    
private:
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket* socket;
};

#endif
