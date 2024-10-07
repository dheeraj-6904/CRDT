# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g

# Libraries
LIBS = -lfltk -lfltk_images -lfltk_forms -lX11 -lpthread

# Source files
SRCS = main.cpp editor_callbacks.cpp menu.cpp

# Executable name
TARGET = editor

# Default rule
all: $(TARGET)

# Rule to build the executable without keeping object files
$(TARGET):
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Clean rule to remove the executable
clean:
	rm -f $(TARGET)

# Phony targets (so 'clean' is not mistaken as a file)
.PHONY: all clean
