# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 `fltk-config --cxxflags` -I/usr/include/boost
LDFLAGS = `fltk-config --ldflags` -lboost_system -lboost_thread

# Executable name
TARGET = editor

# Source files and object files
SRCS = main.cpp TextEditorUI.cpp NetworkManager.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to compile source files (only those that have changed)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS)

clean-all: clean
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean clean-all
