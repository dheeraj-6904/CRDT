#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include <RGA/RGA.h>
class Server {
public:
    Server(const std::string& sharedFolder, const std::string& ipListFile, int port);
    ~Server();
    
    void start();  // Starts the server

private:
    std::string sharedFolder;  // Path to the shared folder
    std::vector<std::string> authorizedIPs;  // List of authorized client IPs
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor* acceptor;
    
    std::map<std::string, std::ofstream> openFiles;  // Keeps track of file streams
    std::map<std::string, RGA> rgaFiles;  // RGA instances for each file

    // Helper functions
    void loadAuthorizedIPs(const std::string& ipListFile);
    void loadRGAFilesFromFolder();
    bool isAuthorized(const std::string& clientIP);
    void handleClient(boost::asio::ip::tcp::socket* socket);
    void sendFileContent(boost::asio::ip::tcp::socket* socket, const std::string& filename);
    void broadcastChange(const std::string& filename, const std::string& content, const std::string& senderIP);

    // Function to handle reading/writing changes
    void handleFileChange(const std::string& filename, const std::string& content);

    // to send shaed forlder for request 100
    void SendSharedFolder(boost::asio::ip::tcp::socket* socket);
    void Delete_File(boost::asio::ip::tcp::socket* socket,std::string filename);
    void Save_File(boost::asio::ip::tcp::socket* socket, std::string filename) ;
    void Create_File(boost::asio::ip::tcp::socket* socket, std::string filename) ;
};

#endif
