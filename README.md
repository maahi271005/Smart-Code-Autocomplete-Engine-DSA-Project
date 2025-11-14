# Smart Code Autocomplete Engine (C++)

A simple data-structures-based project demonstrating how modern IDE autocomplete systems work using:

- **Trie** → Prefix-based search
- **Hashing** → Frequency tracking of code suggestions
- **Heap (Priority Queue)** → Top-K ranked autocomplete results
- **Snippet Expansion** → Replace short keywords with full code templates

## Features
Insert code keywords or phrases  
Autocomplete suggestions based on prefix  
Suggestions ranked by frequency  
Snippet support (e.g., `fori` → `for (int i = 0; i < n; i++)`)  
Practical demonstration of Trie + Hash Map + Heap working together

## Tech Used
- C++
- STL (unordered_map, priority_queue, vector)

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

## Applications
- Code Editors (VS Code, JetBrains)
- Search Engines
- Chatbots
- AI-assisted development tools

---

This project demonstrates a **real-world data structure application** integrating **string algorithms, hashing, and priority queues** into a single practical system.

```md
Run the program → Start typing prefixes → View ranked predictions
