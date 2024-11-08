#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <boost/asio.hpp>

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();
    
    bool connectToServer(const std::string& ip, int port);

    // To send channges to server
    void sendChangeToServer(const std::string& operation);
    
    // Receive data from server
    std::string receiveData();  

    // to get the shared folder
    std::string get_shared_folder(); 

    // to get the file content
    std::string load_file(std::string filename); 

    std::string send_command(std::string command1,std::string command2);
    
private:
    // Do not let end user to access the socket
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket* socket;
};

#endif
