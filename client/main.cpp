#include "TextEditorUI.h"

int main(int argc, char** argv) {
    TextEditorUI textEditor;
    textEditor.show();
    return Fl::run();  // FLTK event loop
}
