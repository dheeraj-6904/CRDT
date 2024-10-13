#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void send_command(const std::string& server_ip, const std::string& server_port, const std::string& command) {
    try {
        // Step 1: Set up Boost Asio I/O service
        boost::asio::io_service io_service;

        // Step 2: Create a resolver to translate the server address and port into a TCP endpoint
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(server_ip, server_port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Step 3: Create a socket and connect to the server
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        // Step 4: Send the command to the server
        boost::asio::write(socket, boost::asio::buffer(command + "\n"));

        // Step 5: Read the server's response
        boost::asio::streambuf response;
        //boost::asio::read_until(socket, response, "\n");

        char reply[2048];
        size_t reply_length = socket.read_some(boost::asio::buffer(reply));
        std::cout<< std::string(reply, reply_length);

        // Convert response to string and display
        // std::istream response_stream(&response);
        // std::string response_data;
        // std::getline(response_stream, response_data);

        //std::cout << "Server Response: " << response_data << std::endl;
        
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    std::string server_ip = "10.40.0.11";  // Replace with the actual IP address
    std::string server_port = "8080";     // Replace with the actual port
    std::string command = "GET file1.txt"; // Replace with the actual command you want to send

    send_command(server_ip, server_port, command);

    return 0;
}
