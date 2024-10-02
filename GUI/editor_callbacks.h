#ifndef EDITOR_CALLBACKS_H
#define EDITOR_CALLBACKS_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Text_Editor.H>  // Ensure this is included

// Function declarations for callback functions
void open_file_cb(Fl_Widget* w, void* data);
void save_file_cb(Fl_Widget* w, void* data);
void delete_file_cb(Fl_Widget* w, void* data);

#endif // EDITOR_CALLBACKS_H
