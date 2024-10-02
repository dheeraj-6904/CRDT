// Import the headers 
#include <FL/Fl_Text_Editor.H>  
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <fstream>
#include <cstdio>  
#include "editor_callbacks.h"


// Function to load a file's content into the text editor
void open_file_cb(Fl_Widget* w, void* data) {
    Fl_Text_Editor* editor = (Fl_Text_Editor*)data;
    const char* filename = fl_file_chooser("Open File", "*.txt", "");
    if (filename) {
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
    if (filename) {
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
    if (filename) {
        std::string message = "Are you sure you want to delete the file:\n%s?";
        int result = fl_choice(message.c_str(), "Cancel", "Delete", 0, filename);
        
        if (result == 1) { // If "Delete" is chosen
            if (std::remove(filename) == 0) {
                fl_message("File deleted successfully.");
            } else {
                fl_alert("Failed to delete the file.");
            }
        }
    }
}
