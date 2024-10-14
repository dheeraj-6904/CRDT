#ifndef TEXT_EDITOR_UI_H
#define TEXT_EDITOR_UI_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Menu_Bar.H>
#include <string>
#include "NetworkManager.h"

class TextEditorUI {
public:
    TextEditorUI();
    ~TextEditorUI();
    
    void show();  // Function to display the UI window
    
private:
    Fl_Window *window;
    Fl_Text_Editor *editor;
    Fl_Text_Buffer *textBuffer;
    Fl_Menu_Bar *menuBar;
    
    NetworkManager* networkManager;  // To handle communication with the server
    
    // Callbacks for menu items
    static void cb_quit(Fl_Widget*, void*);
    static void cb_open(Fl_Widget*, void*);
    static void cb_save(Fl_Widget*, void*);
    static void cb_delete(Fl_Widget*, void*);
    void cb_new(Fl_Widget* widget, void* data);
    // Text editor callbacks
    static void cb_text_changed(int pos, int inserted, int deleted, int restyled, const char* deleted_text, void* data);

    // Helper function to initialize the menu bar
    void createMenuBar();
    
    // Function to handle server connection (asks for IP)
    void connectToServer();
};

#endif
