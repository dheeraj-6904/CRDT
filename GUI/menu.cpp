#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Editor.H>  // Add this line
#include "editor_callbacks.h"
#include "menu.h"

// Function to create the menu bar
Fl_Menu_Bar* create_menu_bar(Fl_Window* window, Fl_Text_Editor* editor) {
    Fl_Menu_Bar* menubar = new Fl_Menu_Bar(0, 0, window->w(), 25);
    
    menubar->add("File/Open", FL_CTRL + 'o', open_file_cb, (void*)editor);
    menubar->add("File/Save", FL_CTRL + 's', save_file_cb, (void*)editor);
    menubar->add("File/Delete", 0, delete_file_cb, 0);
    menubar->add("File/Quit", FL_CTRL + 'q', (Fl_Callback*)[](Fl_Widget*, void*) -> void {
        exit(0);
    });

    return menubar;
}
