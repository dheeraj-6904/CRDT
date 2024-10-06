#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Editor.H>
#include "editor_callbacks.h"
#include "menu.h"


int main(int argc, char **argv) {
    Fl_Window *window = new Fl_Window(800, 600, "Simple CRDT Text Editor");
    
    // Create text editor
    Fl_Text_Editor* editor = new Fl_Text_Editor(0, 25, 800, 575);
    
    // Create menu bar
    Fl_Menu_Bar* menubar = create_menu_bar(window, editor);
    
    // Set up the text buffer
    Fl_Text_Buffer* buffer = new Fl_Text_Buffer();
    editor->buffer(buffer);
    
    // It will set the editor resizable with window
    window->resizable(editor);
    

    //editor->resizable(window);

    window->end();
    window->show(argc, argv);
    return Fl::run();
}

