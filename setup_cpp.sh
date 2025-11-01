#!/bin/bash

# === Smart Autocomplete (C++) Project Scaffolder ===
# Run: bash setup_cpp.sh

ROOT="smart_autocomplete"
echo "📁 Creating C++ project: $ROOT"
mkdir -p $ROOT/{include,src,data,tests}

# --- Header files ---
touch $ROOT/include/{tst.h,minheap.h,lru.h,kmp.h,stack.h,graph.h,freq_store.h,ranker.h}

# --- Source files ---
touch $ROOT/src/{main.cpp,tst.cpp,minheap.cpp,lru.cpp,kmp.cpp,stack.cpp,graph.cpp,freq_store.cpp,ranker.cpp}

# --- Data + Tests ---
touch $ROOT/data/seeds.txt
touch $ROOT/tests/{tst_test.cpp,lru_test.cpp,heap_test.cpp}

# --- README + Makefile ---
touch $ROOT/README.md
cat << 'EOF' > $ROOT/Makefile
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = smart_autocomplete

all: \$(TARGET)

\$(TARGET): \$(OBJ)
	\$(CXX) \$(CXXFLAGS) -o \$@ \$(OBJ)

clean:
	rm -f \$(OBJ) \$(TARGET)
EOF

echo "✅ C++ Smart Autocomplete project scaffold created!"
tree $ROOT

