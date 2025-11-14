CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = smart_autocomplete

# Sources and target for the terminal editor
BASIC_SRCS = basic_editor.cpp src/tst.cpp src/phrase_store.cpp src/freq_store.cpp src/ranker.cpp src/graph.cpp src/minheap.cpp src/lru.cpp src/stack.cpp src/kmp.cpp
BASIC_TARGET = basic_editor

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET) $(BASIC_TARGET)

.PHONY: clean all
