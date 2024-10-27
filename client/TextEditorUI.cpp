#include "TextEditorUI.h"
#include "FileManager.h"
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Browser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Select_Browser.H>
#include <iostream>
#include "NetworkManager.h"
#include <FL/Fl_Input.H>
#include<FL/Fl.H>
#include <nlohmann/json.hpp>

//global variables
// define IP and port as globle vars
std::string ip, port,filename = "";

TextEditorUI::TextEditorUI() {
    // Create the main window and editor
    window = new Fl_Window(800, 600, "Collaborative Text Editor");
    textBuffer = new Fl_Text_Buffer();
    editor = new Fl_Text_Editor(10, 40, 780, 550);
    editor->buffer(textBuffer);

    // Set callback for text changes
    textBuffer->add_modify_callback(cb_text_changed, this);  // will do main thing ⭐⭐✡️
    
    // Create the menu bar
    createMenuBar();

    // Initialize NetworkManager (connect to server)
    networkManager = new NetworkManager();
    connectToServer();
}

TextEditorUI::~TextEditorUI() {
    delete editor;
    delete textBuffer;
    delete window;
    delete networkManager;
}

void TextEditorUI::createMenuBar() {
    menuBar = new Fl_Menu_Bar(0, 0, 800, 40);
    menuBar->add("&File/New", FL_CTRL + 'n', cb_new, this);
    menuBar->add("&File/Open", FL_CTRL + 'o', cb_open, this);
    menuBar->add("&File/Save", FL_CTRL + 's', cb_save, this);
    menuBar->add("&File/Delete",FL_CTRL + 'd' , cb_delete, this);
    menuBar->add("Quit", FL_CTRL + 'q', cb_quit, this);
}

void TextEditorUI::show() {
    window->end();
    window->show();
}

// Func to split ip and port
void split_ip_port(const std::string& input, std::string& ip, std::string& port) {
    size_t colon_pos = input.find(':');
    if (colon_pos != std::string::npos) {
        ip = input.substr(0, colon_pos);      
        port = input.substr(colon_pos + 1);     
    } else {
        fl_alert("Trying to connect with default ip and port");
        ip = input;   // If no port is provided, just use the input as IP
        port = "8080"; // and 8080 as the dafault port
    }
}

void TextEditorUI::connectToServer() {
    std::string input = fl_input("Enter server IP:", "10.40.0.36:8070");

    split_ip_port(input, ip, port); // Will be changed in place
    if (!networkManager->connectToServer(ip, std::stoi(port))) {
        fl_alert("Unable to connect to server!");
        //window->hide(); // Hide the window before exiting
        exit(0); // exit if not connected
    }
}

// Menu callbacks
// void TextEditorUI::cb_open(Fl_Widget* widget, void* data) {
//     TextEditorUI* editor = (TextEditorUI*) data;
//     Fl_Native_File_Chooser chooser;
//     chooser.title("Open File");
//     //chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);

//     //request the server

//     if (chooser.show() == 0) {
//         std::string filename = chooser.filename();
//         FileManager fileManager;
//         std::string content = fileManager.openFile(filename);
//         editor->textBuffer->text(content.c_str());
//     }
// }


void TextEditorUI::cb_open(Fl_Widget* widget, void* data) {
    TextEditorUI* editor = (TextEditorUI*)data;

    // Get the list of remote files (separated by \n character)
    std::string remoteFiles = editor->networkManager->get_shared_folder(); 
    
    // Create a custom window for file selection
    Fl_Window* window = new Fl_Window(300, 200, "Select Remote File");
    Fl_Select_Browser* browser = new Fl_Select_Browser(20, 20, 260, 140);
    
    // Add remote files to the browser
    std::istringstream ss(remoteFiles);
    std::string line;
    while (std::getline(ss, line)) {
        browser->add(line.c_str());
    }

    int selected = browser->value();  // Get the selected item's index

    // Set user_data to pass both browser and editor
    browser->user_data(editor);
    //browser->callback(item_selected_callback);

    // Add an "OK" button to confirm selection
    Fl_Button* ok_button = new Fl_Button(100, 170, 100, 25, "Open");

    //Set user_data to pass both browser and editor
    browser->user_data(editor); // Associate the editor with the browser
    
    //Use a non-capturing lambda or static callback
    ok_button->callback([](Fl_Widget* widget, void* data) {
        Fl_Browser* browser = static_cast<Fl_Browser*>(widget->parent()->child(0));  // Get the browser from the parent window
        TextEditorUI* editor = static_cast<TextEditorUI*>(browser->user_data()); // Retrieve editor from user_data

        int selected_index = browser->value();
        if (selected_index > 0) {
            std::string selected_file = browser->text(selected_index);
            filename = selected_file;
            // Close the window after file selection
            browser->window()->hide();

            // Handle opening the selected remote file
            std::string content = editor->networkManager->load_file(selected_file); // Access editor
            editor->textBuffer->text(content.c_str());  // Display the content in the text editor
            //std::cout<<content;

        } else {
            fl_alert("No file selected!");
        }
    });

    window->end();
    window->show();

    // Block execution until the user selects a file and clicks OK
    window->set_modal();
    Fl::run();
}

// to save a file
void TextEditorUI::cb_save(Fl_Widget* widget, void* data) {
    TextEditorUI* editor = (TextEditorUI*) data;
        if(filename == ""){
        fl_alert("you must open a file before saving");
        return;
    }
    std::string responce = editor->networkManager->send_command("SAVE",filename +"-"+std::string(editor->textBuffer->text()));
    // diaplay the responce
    fl_alert(responce.c_str());

}

void TextEditorUI::cb_delete(Fl_Widget* widget, void* data) {
    if(filename == ""){
        fl_alert("you must open a file before deleting");
        return;
    }
    TextEditorUI* editor = (TextEditorUI*) data;
    std::string responce = editor->networkManager->send_command("DELETE",filename);
    if(responce == "100"){
        fl_alert(" File deleted susscesfully");
    }
    else{
        fl_alert(responce.c_str());
    }
}

// To create new file (yet to be tested)
void TextEditorUI::cb_new(Fl_Widget* widget, void* data) {
    try {
        TextEditorUI* editor = (TextEditorUI*) data;
        
        // If some file is opened, save it first
        if (filename != "") {
            editor->networkManager->send_command("SAVE", filename);
        }

        // Ask the user to enter the filename for the new file
        const char* input = fl_input("Enter new filename:", "");
        if (input == nullptr || std::string(input).empty()) {
            fl_alert("No filename provided!");
            return;  // Exit if no filename is provided
        }

        Fl::flush();
        std::string new_filename = std::string(input);

        // Now send command to save the file
        std::string response = editor->networkManager->send_command("CREATE", new_filename + ".txt");
        if (response == "100") {
            fl_alert((new_filename + " created successfully!").c_str());
        } else {
            fl_alert((response.c_str()));
        }
    } catch (const std::exception& e) {
        fl_alert(("An error occurred: " + std::string(e.what())).c_str());
    } catch (...) {
        fl_alert("An unknown error occurred!");
    }
}

void TextEditorUI::cb_quit(Fl_Widget* widget, void* data) {
    TextEditorUI* editor = (TextEditorUI*) data;

    // ask once if the user is sure to exit
    int response = fl_choice("Are you sure you want to exit?", "No", "Yes", 0);

    // If user selects "Yes" (button index 1), proceed to quit
    if (response == 1) {
        std::string responce = editor->networkManager->send_command("QUIT", "NILL");
        exit(0);
    }
    // Otherwise, do nothing (in case of responce equal to 0)
}


// Text change callback
// void TextEditorUI::cb_text_changed(int pos, int inserted, int deleted, int restyled, const char* deleted_text, void* data) {
//     TextEditorUI* editor = (TextEditorUI*) data;
//     int x = editor->editor->insert_position();  // Current cursor position
//     int y = 0; // Could map row/column later

//     std::string changeOperation = editor->textBuffer->text_range(pos, pos + inserted);
//     editor->networkManager->sendChangeToServer(changeOperation, x, y);
// }



void TextEditorUI::cb_text_changed(int pos, int inserted, int deleted, int restyled, const char* deleted_text, void* data) {
    TextEditorUI* editor = (TextEditorUI*) data;
    int cursor_position = editor->editor->insert_position();  // Current cursor position

    // Prepare message
    std::string message; 

    if (inserted > 0) {
        // Insert operation
        std::string changeOperation = editor->textBuffer->text_range(pos, pos + inserted);
        message = "INSERT" +filename +" "+ changeOperation + " " + std::to_string(cursor_position) ; 
    } 
    else if (deleted > 0) {
        // Delete operation
        message = std::string("DELETE "  + filename +" "+ std::to_string(cursor_position) ); 
    }

    // Send message to server
    editor->networkManager->sendChangeToServer(message);
}


