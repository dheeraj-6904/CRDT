#ifndef MENU_H
#define MENU_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Editor.H>

// Function declaration for creating the menu bar
Fl_Menu_Bar* create_menu_bar(Fl_Window* window, Fl_Text_Editor* editor);

#endif // MENU_H
