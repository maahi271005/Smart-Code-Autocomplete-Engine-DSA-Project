# Smart Code Autocomplete Engine (C++)

The Smart Code Autocomplete Engine is a C++ project designed to suggest intelligent code completions in real time — similar to how modern IDEs like VS Code or IntelliJ offer autocomplete suggestions.
It applies core Data Structures and Algorithms (DSA) concepts such as Tries, Heaps, and LRU (Least Recently Used) caching to efficiently predict the next most probable code tokens based on user input frequency and context.

## Idea
When a programmer starts typing part of a keyword or function name, the engine quickly:
Searches through a Trie (prefix tree) for all words starting with that prefix.
Uses a Heap to rank suggestions by frequency or relevance.
Employs an LRU Cache to prioritize recently used or selected completions, making the system adaptive over time.

This results in fast, memory-efficient, and intelligent autocomplete suggestions that simulate the logic behind real-world code editors — but built purely from scratch using fundamental DSA concepts.


## DSA Concepts used

### 1. Trie (Prefix Tree)

- Files: tst.h, tst.cpp, tst_test.cpp
- Used for storing and retrieving words efficiently based on their prefixes.
- Enables O(L) time complexity lookups (where L = length of prefix).
- Supports real-time suggestions as the user types each character.

🔹 Concepts used: String manipulation, recursion, tree traversal, prefix-based searching.

### 2. Min-Heap / Max-Heap

- Files: minheap.h, minheap.cpp, heap_test.cpp
- Maintains the top N most frequent or relevant words efficiently.
- Provides constant-time access to the best-ranked suggestion.
- Used during ranking and sorting of autocomplete results.

🔹 Concepts used: Binary heap operations, priority queue logic, partial sorting.

### 3. LRU (Least Recently Used) Cache
- Files: lru.h, lru.cpp, lru_test.cpp
- Stores recently used suggestions for quick access.
- Improves responsiveness by avoiding repetitive Trie lookups.
- Implemented using a combination of doubly linked list + hash map.

🔹 Concepts used: Linked lists, hashing, cache eviction policy.

### 4. KMP (Knuth–Morris–Pratt) Algorithm
- Files: kmp.h, kmp.cpp
- Used for efficient substring pattern matching between typed input and stored code tokens.
- Ensures fast lookup of partial matches even in large word lists.

🔹 Concepts used: Prefix table computation, linear-time pattern searching.

### 5. Graph Data Structure

- Files: graph.h, graph.cpp
- Represents relationships between tokens or code components.
- Can model transitions between function calls or variable dependencies for context-aware suggestions.

🔹 Concepts used: Adjacency list representation, graph traversal (BFS/DFS).

### 6. Stack

- Files: stack.h, stack.cpp
- Used internally for recursive operations, backtracking, or maintaining function call hierarchies.
- Simplifies control flow during traversal or undo operations in text editing logic.

🔹 Concepts used: LIFO operations, template-based generic stack implementation.

### 7. Ranking System
- Files: ranker.h, ranker.cpp
- Combines frequency and recency scores from Trie, Heap, and LRU cache to rank autocomplete suggestions.
- Implements a weighted scoring system for realistic, adaptive predictions.

🔹 Concepts used: Comparator functions, dynamic sorting, frequency-based ranking.

### 8. Frequency Storage
- Files: freq_store.h, freq_store.cpp, frequency.txt
- Keeps track of how often each word is used.
- Updates dynamically after every suggestion selection, making the model “learn” over time.

🔹 Concepts used: File handling, hash mapping, frequency analysis.

---

## Features
- Insert code keywords or phrases  
- Autocomplete suggestions based on prefix  
- Suggestions ranked by frequency  
- Snippet support (e.g., `fori` → `for (int i = 0; i < n; i++)`)  
- Practical demonstration of Trie + Hash Map + Heap working together

## Tech Stack Used

| Category | Technologies / Tools | Description |
|-----------|----------------------|--------------|
| **Language** | C++ (C++17 Standard) | Core implementation of all modules, data structures, and algorithms. |
| **Build System** | GNU Make (Makefile) | Used for compiling multiple source files and linking into a single executable. |
| **Compiler** | GCC / G++ | To compile and build the C++ source files efficiently. |
| **Version Control** | Git + GitHub | For code management, versioning, and collaboration among team members. |
| **Testing Framework** | Custom test files (`tests/`) | Unit testing for core modules like Trie, LRU, and Heap. |
| **Data Storage** | Text files (`data/words.txt`, `data/frequency.txt`) | Stores training words and their frequency for suggestion ranking. |
| **Editor / IDE** | VS Code | Primary development environment for coding, debugging, and project organization. |

---


## How It Works
1. User enters keywords (e.g., `print`, `printf`, `private`, etc.)
2. Trie stores all words for fast prefix lookup
3. Hash map tracks how often each word is used
4. Heap finds the most frequent matches for a prefix
5. Snippets expand small abbreviations into full code blocks

## Example Usage
- Type: `pri`
- Suggestions: `print`, `printf`, `private` (ranked)
- Type: `fori`
- Expanded snippet: `for (int i = 0; i < n; i++)`

## Setup Instructions
---
### 1. Clone the repository
- git clone https://github.com/maahi271005/Smart-Code-Autocomplete-Engine-DSA-Project
- cd smart_autocomplete

---


### 2. (Optional) Create and activate virtual environment

If your project uses Python utilities or scripts (e.g., preprocessing):

- python3 -m venv dsavenv
- source dsavenv/bin/activate

---

### 3. Install build tools (for Linux/Ubuntu)
- sudo apt update
- sudo apt install build-essential

---
### 4. Build the project

Use the provided Makefile:

- make clean
- make


This will generate the executable:

- ./smart_autocomplete

---
### 5. Run the program
./smart_autocomplete


If you encounter GLIBCXX_3.4.32 not found, simply rebuild the project on your machine (make clean && make) to link against your local C++ standard library.

---
## Running Tests

To verify components:

- g++ tests/heap_test.cpp -o heap_test && ./heap_test
- g++ tests/lru_test.cpp -o lru_test && ./lru_test
- g++ tests/tst_test.cpp -o tst_test && ./tst_test

---
## Applications
- Code Editors (VS Code, JetBrains)
- Search Engines
- Chatbots
- AI-assisted development tools

---


## Contributors 
| Name | GitHub |
|------|---------|
| Tanisha Ray | [![GitHub](https://img.shields.io/badge/-@tanisharay-181717?logo=github&style=flat)](https://github.com/coderTanisha22) |
| Maahi Ratanpara | [![GitHub](https://img.shields.io/badge/-@maahiratanpara-181717?logo=github&style=flat)](https://github.com/maahi271005) |
| Anika Sharma | [![GitHub](https://img.shields.io/badge/-@anikasharma-181717?logo=github&style=flat)](https://github.com/Anika438) |
| Akshita Maheshwari | [![GitHub](https://img.shields.io/badge/-@akshitamaheshwari-181717?logo=github&style=flat)](https://github.com/AkshitaM1234) |


---
## Educational Purpose
This project demonstrates the application of DSA in a real-world scenario — showing how core structures like tries, heaps, and caches can combine to form an intelligent system used in everyday developer tools.
//test



