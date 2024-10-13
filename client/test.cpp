#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/fl_message.H>
#include <string>
#include <sstream>

// Callback function that gets triggered when an item is selected
void item_selected_callback(Fl_Widget* widget, void*) {
    Fl_Browser* browser = (Fl_Browser*)widget;
    int selected = browser->value();  // Get the selected item's index

    if (selected > 0) { // Index 0 means no item selected
        const char* selected_item = browser->text(selected);  // Get the selected item text
        fl_message("You selected: %s", selected_item);  // Display a message with the selected item
    }
}

// Helper function to split a string by a delimiter and add items to the browser
void add_items_to_browser(Fl_Browser* browser, const std::string& input_string) {
    std::istringstream stream(input_string);
    std::string line;
    
    while (std::getline(stream, line, '\n')) {
        browser->add(line.c_str());  // Add each line as an item in the browser
    }
}

int main(int argc, char **argv) {
    // Example input string (items separated by \n)
    std::string items = "Apple\nBanana\nCherry\nDate\nElderberry";

    // Create the FLTK window
    Fl_Window* window = new Fl_Window(300, 200, "Select an Item");

    // Create a browser widget to display items
    Fl_Select_Browser* browser = new Fl_Select_Browser(20, 20, 260, 140);
    
    // Add items to the browser
    add_items_to_browser(browser, items);

    // Set a callback function for when an item is selected
    browser->callback(item_selected_callback);

    // Show the window
    window->end();
    window->show(argc, argv);

    // Start the FLTK event loop
    return Fl::run();
}