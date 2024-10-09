#include <FL/Fl_Text_Editor.H>  
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <fstream>
#include <cstdio>  
#include<iostream>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/fl_ask.H>
#include <fstream>
#include <vector>
#include <filesystem>
#include <boost/asio.hpp>
#include "editor_callbacks.h" // the header

// Nessasities to check if the IP is valid or not
#ifdef _WIN32  // for the windoes
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else // for the linux users
    #include <arpa/inet.h>
#endif

// curent filename variable
std::string filename = ""; // Use empty string instead of NULL

// Function to load a file's content into the text editor
// Function to load a file's content into the text editor
void open_file_cb(Fl_Widget* w, void* data) {
    Fl_Text_Editor* editor = (Fl_Text_Editor*)data;
    filename = fl_file_chooser("Open File", "*.txt", "");
    if (!filename.empty()) {  // Check if a file was selected
        std::ifstream file(filename);
        if (file) {
            std::string content((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
            editor->buffer()->text(content.c_str());
            file.close();
        } else {
            fl_alert("Failed to open the file.");
        }
    }
}

// Function to save the text editor's content to a file
void save_file_cb(Fl_Widget* w, void* data) {
    Fl_Text_Editor* editor = (Fl_Text_Editor*)data;
    const char* filename = fl_file_chooser("Save File", "*.txt", "");
    if (filename && strlen(filename) > 0) {  // Ensure valid file selected
        std::ofstream file(filename);
        if (file) {
            file << editor->buffer()->text();
            file.close();
        } else {
            fl_alert("Failed to save the file.");
        }
    }
}

// Function to delete a selected file
void delete_file_cb(Fl_Widget* w, void* data) {
    const char* filename = fl_file_chooser("Delete File", "*.txt", "");
    if (filename && strlen(filename) > 0) {  // Check if a file was selected
        int result = fl_choice("Are you sure you want to delete the file:\n%s?", "Cancel", "Delete", nullptr, filename);
        if (result == 1) { // If "Delete" is chosen
            if (std::remove(filename) == 0) {
                fl_message("File deleted successfully.");
            } else {
                fl_alert("Failed to delete the file.");
            }
        }
    }
}


// Function to trim whitespace from a string (helper function)
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Function to validate IPv4 and IPv6 addresses
bool is_valid_ip(const std::string& ip) {
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;

    // Try IPv4
    int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
    if (result == 1) {
        return true;
    }

    // Try IPv6
    result = inet_pton(AF_INET6, ip.c_str(), &(sa6.sin6_addr));
    if (result == 1) {
        return true;
    }

    return false;
}

// The collab_popup function
void collab_popup() { 
    // Check if a file has been opened
    if (filename == "") {
        fl_alert("First you must open a file!!");
        return;
    }

    // Define the original file path and extract only the filename (without extension)
    std::filesystem::path original_file_path(filename);
    std::string file_stem = original_file_path.stem().string();  // Get the filename without extension, e.g., "task"

    // Define the full path for the collab file in the target directory
    std::string collab_dir = std::filesystem::current_path().string() +"/collabs/";  //poe
    std::string new_file_path = collab_dir + file_stem + "_collab.txt";

    // Ensure the collabs directory exists
    if (!std::filesystem::exists(collab_dir)) {
        std::filesystem::create_directory(collab_dir);
    }

    // Check if the collab file exists for the selected file; if not, create it
    if (!std::filesystem::exists(new_file_path)) {
        std::ofstream new_file(new_file_path);

        if (!new_file) {
            fl_alert("Failed to create the file: %s", new_file_path.c_str());
            return;
        }
        new_file.close();
    }

    // Create a new window for collaboration
    Fl_Window* popup = new Fl_Window(400, 300, "Collaboration IPs");

    // Create a multi-browser to display IP addresses
    Fl_Multi_Browser* ip_list = new Fl_Multi_Browser(20, 20, 360, 200);
    ip_list->type(FL_MULTI_BROWSER);

    // Load existing IPs from the file
    std::ifstream infile(new_file_path);
    std::string line;
    while (std::getline(infile, line)) {
        line = trim(line);
        if (!line.empty()) {
            ip_list->add(line.c_str());
        }
    }
    infile.close();

    // input field for adding new IP
    Fl_Input* add_input = new Fl_Input(20, 230, 200, 30, "Add IP:");
    
    // Add, Add and Delete buttons
    Fl_Button* add_button = new Fl_Button(240, 230, 60, 30, "Add");
    Fl_Button* delete_button = new Fl_Button(320, 230, 60, 30, "Delete");

    // Callback for the Add button
    add_button->callback([](Fl_Widget*, void* data) {
        Fl_Window* popup = (Fl_Window*)data;
        Fl_Input* input = (Fl_Input*)popup->child(1); // Second child is Fl_Input
        Fl_Multi_Browser* list = (Fl_Multi_Browser*)popup->child(0); // First child is the list
        std::string new_ip = trim(input->value());

        if (new_ip.empty()) {
            fl_alert("IP address cannot be empty.");
            return;
        }
        
        // Validate the IP address(In case of typos)
        if (!is_valid_ip(new_ip)) {
            fl_alert("Invalid IP address. Please enter a valid IPv4 or IPv6 address.");
            return;
        }

        // Check for duplicate IP's
        for (int i = 1; i <= list->size(); ++i) {
            if (new_ip == list->text(i)) {
                fl_alert("This IP address is already in the list.");
                return;
            }
        }

        list->add(new_ip.c_str());
        input->value("");

        // Update file path
        std::filesystem::path original_file_path(filename);
        std::string file_stem = original_file_path.stem().string();
        std::string collab_dir = std::filesystem::current_path().string() +"/collabs/";  //poe
        std::string new_file_path = collab_dir + file_stem + "_collab.txt";

        std::ofstream outfile(new_file_path, std::ios::app);
        if (outfile) {
            outfile << new_ip << "\n";
            outfile.close();
        } else {
            fl_alert("Failed to write to the file.");
        }
    }, popup);  // Pass popup as data

    // Callback for the Delete button
    delete_button->callback([](Fl_Widget*, void* data) {
        Fl_Window* popup = (Fl_Window*)data;
        Fl_Multi_Browser* list = (Fl_Multi_Browser*)popup->child(0); // First child is the list
        int selected = list->value();
        if (selected <= 0) {
            fl_alert("Please select an IP to delete.");
            return;
        }

        std::string selected_ip = list->text(selected);
        list->remove(selected);

        // Update file path
        std::filesystem::path original_file_path(filename);
        std::string file_stem = original_file_path.stem().string();
        std::string collab_dir = std::filesystem::current_path().string() +"/collabs/";  //poe

        std::string new_file_path = collab_dir + file_stem + "_collab.txt";

        std::vector<std::string> ips;
        std::ifstream infile(new_file_path);
        std::string line;
        while (std::getline(infile, line)) {
            line = trim(line);
            if (line != selected_ip && !line.empty()) {
                ips.push_back(line);
            }
        }
        infile.close();

        std::ofstream outfile(new_file_path, std::ios::trunc);
        if (outfile) {
            for (const auto& ip : ips) {
                outfile << ip << "\n";
            }
            outfile.close();
        } else {
            fl_alert("Failed to update the file.");
        }
    }, popup);  // Pass popup as data

    popup->end();
    popup->show();
}
