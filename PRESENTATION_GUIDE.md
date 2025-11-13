# Smart Code Autocomplete Engine - Presentation Guide

## 📋 Table of Contents
1. [Project Overview](#project-overview)
2. [Data Structures Used](#data-structures-used)
3. [System Architecture](#system-architecture)
4. [Live Demo Script](#live-demo-script)
5. [Technical Deep Dive](#technical-deep-dive)

---

## 🎯 Project Overview

**Project Name:** Smart Code Autocomplete Engine
**Purpose:** Intelligent code completion system that learns from user behavior
**Key Features:**
- Context-aware suggestions
- Learning system (frequency tracking)
- Co-occurrence pattern detection
- Undo/Redo functionality
- Substring search capability

---

## 🗂️ Data Structures Used

### 1. **Ternary Search Tree (TST)**
**Location:** `include/tst.h`, `src/tst.cpp`

**Purpose:** Efficient prefix-based word storage and retrieval

**Why TST?**
- Space-efficient compared to Trie (especially for large alphabets)
- Faster than hash tables for prefix searches
- O(log n) search complexity per character

**Structure:**
```
TSTNode {
    char data
    bool isEndOfString
    TSTNode* left   // lesser characters
    TSTNode* eq     // equal/next character
    TSTNode* right  // greater characters
}
```

**Operations:**
- `insert(word)` - Add word to dictionary: O(m log n) where m = word length
- `search(word)` - Check if exact word exists: O(m log n)
- `prefixSearch(prefix, k)` - Find all words with prefix: O(m log n + k)

**Real-world Usage in Code:**
```cpp
// In main.cpp line 85
std::vector<std::string> candidates = tst.prefixSearch(prefix, k * 2);
```

---

### 2. **Min Heap (Priority Queue)**
**Location:** `include/minheap.h`, `src/minheap.cpp`

**Purpose:** Efficiently maintain top-K ranked suggestions

**Why Min Heap?**
- Keep only the K best suggestions without sorting entire list
- O(log k) insertion time
- Efficient memory usage (only store K items)

**Structure:**
```
MinHeap {
    vector<pair<double, string>> heap  // (score, word)
    int maxSize  // K value
}
```

**Operations:**
- `insert(score, word)` - Add to heap: O(log k)
- `extractMin()` - Remove lowest score: O(log k)
- `getAll()` - Get sorted results: O(k log k)

**Real-world Usage in Code:**
```cpp
// In ranker.cpp line 30
MinHeap heap(k);
for (const auto& token : candidates) {
    double score = computeScore(token);
    heap.insert(score, token);  // Keeps only top K
}
```

**Heap Property:** Parent node score ≤ child node scores (Min Heap)

---

### 3. **Stack (Undo/Redo)**
**Location:** `include/stack.h`, `src/stack.cpp`

**Purpose:** Track editing history for undo/redo operations

**Why Two Stacks?**
- One stack for undo operations
- One stack for redo operations
- O(1) push and pop operations

**Structure:**
```
UndoRedoStack {
    stack<pair<int, string>> undoStack  // (position, text)
    stack<pair<int, string>> redoStack
}
```

**Operations:**
- `pushInsert(pos, text)` - Record action: O(1)
- `undo()` - Revert last action: O(1)
- `redo()` - Restore undone action: O(1)

**Real-world Usage in Code:**
```cpp
// In main.cpp line 114
undoRedo.pushInsert(0, token);

// In main.cpp line 127
auto [pos, token] = undoRedo.undo();
```

**Stack Behavior:**
```
Accept "for" → undoStack: [("for", 0)]
Accept "int" → undoStack: [("for", 0), ("int", 1)]
Undo        → undoStack: [("for", 0)], redoStack: [("int", 1)]
Redo        → undoStack: [("for", 0), ("int", 1)], redoStack: []
```

---

### 4. **Graph (Co-occurrence)**
**Location:** `include/graph.h`, `src/graph.cpp`

**Purpose:** Track which tokens frequently follow each other

**Why Directed Weighted Graph?**
- Models token relationships (e.g., "for" often followed by "int")
- Weighted edges show frequency of co-occurrence
- Enables context-aware suggestions

**Structure:**
```
CooccurrenceGraph {
    unordered_map<string, map<string, int>> adjacencyList
    // from_token → { to_token → weight }
}
```

**Operations:**
- `addEdge(from, to)` - Increment co-occurrence: O(log m) where m = neighbors
- `getBoost(from, to)` - Get relationship strength: O(log m)
- `display()` - Show graph structure: O(V + E)

**Real-world Usage in Code:**
```cpp
// In main.cpp line 111
if (!lastAccepted.empty()) {
    graph.addEdge(lastAccepted, token);
}

// In ranker.cpp line 17
graphBoost = graph->getBoost(lastToken, token);
```

**Example Graph:**
```
"for" → "int"(5), "char"(2), "auto"(3)
"int" → "main"(8), "i"(6), "x"(4)
"if" → "true"(3), "false"(2), "x"(7)
```

---

### 5. **Hash Map (Multiple Uses)**
**Location:** Multiple files

#### a) **LRU Cache** (`include/lru.h`, `src/lru.cpp`)
**Purpose:** Cache recent prefix search results

**Structure:**
```
LRUCache {
    unordered_map<string, Node*> cacheMap  // O(1) lookup
    Node* head, tail  // Doubly-linked list for LRU ordering
}
```

**Why Hash Map + Doubly Linked List?**
- O(1) cache lookup via hash map
- O(1) LRU updates via linked list
- Efficient memory management

**Real-world Usage:**
```cpp
// In main.cpp line 76
if (cache.exists(prefix)) {
    auto cached = cache.get(prefix);  // O(1) retrieval
    return cached;
}
```

#### b) **Frequency Store** (`include/freq_store.h`, `src/freq_store.cpp`)
**Purpose:** Track how often each token is used

**Structure:**
```
FreqStore {
    unordered_map<string, int> frequencies  // token → count
}
```

**Operations:**
- `get(token)` - Get frequency: O(1)
- `bump(token, amount)` - Increase frequency: O(1)

**Real-world Usage:**
```cpp
// In main.cpp line 108
freqStore.bump(token, 1);  // Increment usage

// In ranker.cpp line 13
double freqScore = freqStore->get(token);  // For scoring
```

#### c) **Graph Adjacency List** (Already covered above)

---

### 6. **String Matching Algorithm (KMP)**
**Location:** `include/kmp.h`, `src/kmp.cpp`

**Purpose:** Efficient substring search in words

**Why KMP?**
- Linear time complexity: O(n + m)
- Better than naive O(n * m) approach
- Useful for fuzzy/substring matching

**Structure:**
```
KMP {
    static vector<int> computeLPS(pattern)  // Longest Prefix Suffix
    static bool contains(text, pattern)
    static vector<int> findAll(text, pattern)
}
```

**Real-world Usage in Code:**
```cpp
// In main.cpp line 52
if (KMP::contains(word, prefix)) {
    results.push_back(word);  // Substring match
}
```

**LPS Table Example:**
```
Pattern: "aba"
LPS:     [0, 0, 1]

Pattern: "abcab"
LPS:     [0, 0, 0, 1, 2]
```

---

## 🏗️ System Architecture

### Data Flow Diagram
```
User Input "for"
    ↓
[TST] prefixSearch("for") → ["for", "foreach", "format", ...]
    ↓
[KMP] (if <3 results) substringSearch → additional matches
    ↓
[Ranker] computeScore for each candidate
    ├── [FreqStore] get frequency score
    └── [Graph] get co-occurrence boost
    ↓
[MinHeap] maintain top K results → sorted list
    ↓
[LRU Cache] store results for "for"
    ↓
Display to User
    ↓
User accepts suggestion → update [FreqStore], [Graph], [Stack]
```

### Component Interaction
```
┌─────────────────────────────────────────────────┐
│           AutocompleteEngine                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐      │
│  │   TST    │  │ LRUCache │  │ FreqStore│      │
│  │ (Trie)   │  │(HashMap) │  │(HashMap) │      │
│  └──────────┘  └──────────┘  └──────────┘      │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐      │
│  │  Graph   │  │  Ranker  │  │UndoRedo  │      │
│  │(Directed)│  │(MinHeap) │  │ (Stack)  │      │
│  └──────────┘  └──────────┘  └──────────┘      │
│  ┌──────────┐                                    │
│  │   KMP    │                                    │
│  │(Pattern) │                                    │
│  └──────────┘                                    │
└─────────────────────────────────────────────────┘
```

---

## 🎬 Live Demo Script

### Demo 1: Basic Autocomplete (TST)
**Objective:** Show prefix search using Ternary Search Tree

**Steps:**
1. Start the program: `./smart_autocomplete`
2. Type: `for`
3. **Explain:** TST quickly finds all words starting with "for"
4. Show: Suggestion appears instantly
5. Accept by typing: `1`

**What to Say:**
> "The Ternary Search Tree efficiently stores 88 tokens. When I type 'for', it traverses the tree comparing characters. The left/right branches handle alphabetical ordering, while equal branches follow the search path. This gives us O(log n) time per character."

---

### Demo 2: Top-K Selection (Min Heap)
**Objective:** Demonstrate heap-based ranking

**Steps:**
1. Type: `p` (will match: print, printf, println, private, protected, public, etc.)
2. **Explain:** Multiple candidates, but only top 5 shown
3. Show the scores next to each suggestion

**What to Say:**
> "When we have many matches, the Min Heap maintains only the top K suggestions efficiently. As we compute scores (frequency + context), the heap keeps the K highest scores. This is much faster than sorting all candidates - O(n log k) instead of O(n log n)."

---

### Demo 3: Frequency Learning (Hash Map)
**Objective:** Show adaptive learning

**Steps:**
1. Type: `int` → Accept it: `1`
2. Type: `:bump int` → Manually increase frequency by 5
3. Type: `int` again → **Show score increased**
4. Type: `i` → Notice "int" ranks higher now

**What to Say:**
> "The Frequency Store uses a hash map for O(1) lookups and updates. Each time you accept 'int', its frequency increases. The :bump command simulates heavy usage. Now 'int' has a higher score and appears more prominently in suggestions."

---

### Demo 4: Context Awareness (Graph)
**Objective:** Demonstrate co-occurrence learning

**Steps:**
1. Type: `for` → Accept: `1`
2. Type: `int` → Accept: `1`
3. Type: `for` → Accept: `1`
4. Type: `int` → Accept: `1`
5. Type: `:graph` → **Show the edge from "for" to "int"**
6. Type: `for` → Accept: `1`
7. Type: `i` → **Notice "int" gets context boost**

**What to Say:**
> "The Co-occurrence Graph is a directed weighted graph tracking token sequences. Each edge weight shows how often one token follows another. When 'for' is the last accepted token, the graph boosts 'int's score because they frequently appear together. This makes suggestions context-aware!"

**Graph Output Example:**
```
=== Co-occurrence Graph ===
for → int(3)
```

---

### Demo 5: Undo/Redo (Stack)
**Objective:** Show stack-based history

**Steps:**
1. Type: `for` → Accept: `1` → Output: "Accepted: for"
2. Type: `int` → Accept: `1` → Output: "Accepted: int"
3. Type: `auto` → Accept: `1` → Output: "Accepted: auto"
4. Type: `:undo` → Output: "Undo: Removed 'auto'"
5. Type: `:undo` → Output: "Undo: Removed 'int'"
6. Type: `:redo` → Output: "Redo: Restored 'int'"
7. Type: `:redo` → Output: "Redo: Restored 'auto'"

**What to Say:**
> "We use two stacks for undo/redo. The undo stack stores all accepted tokens. When you undo, we pop from undo stack and push to redo stack. When you redo, we pop from redo stack and push back to undo stack. This gives us O(1) operations and unlimited history."

**Stack Visualization:**
```
Initial:     undoStack: []                  redoStack: []
Accept "for": undoStack: ["for"]            redoStack: []
Accept "int": undoStack: ["for", "int"]     redoStack: []
Undo:        undoStack: ["for"]            redoStack: ["int"]
Undo:        undoStack: []                 redoStack: ["int", "for"]
Redo:        undoStack: ["for"]            redoStack: ["int"]
```

---

### Demo 6: Substring Search (KMP)
**Objective:** Show pattern matching algorithm

**Steps:**
1. Type: `:toggle_contains` → Enable substring search
2. Type: `main` → **Show matches like "main", "domain", "remain"**
3. **Explain:** KMP finds pattern anywhere in word, not just prefix

**What to Say:**
> "KMP (Knuth-Morris-Pratt) algorithm enables substring matching. Instead of checking every position naively (O(nm) time), KMP uses a preprocessed LPS array to skip redundant comparisons, achieving O(n+m) time. This allows flexible 'contains' searches beyond just prefix matching."

---

### Demo 7: Caching (LRU + HashMap)
**Objective:** Show cache effectiveness

**Steps:**
1. Type: `for` → First time (cache miss)
2. Accept or press Enter to skip
3. Type: `for` again → **Instant from cache**
4. Type many different prefixes to fill cache
5. Type: `for` again → Still cached if within LRU limit

**What to Say:**
> "The LRU Cache uses a hash map for O(1) lookups and a doubly-linked list to track recency. When you search 'for', results are cached. On repeated searches, we skip TST traversal entirely. The LRU policy ensures we keep the most recently used 50 items, evicting the least recently used when full."

---

## 🔍 Technical Deep Dive

### 1. TST vs Trie Comparison

| Feature | Trie | TST (Our Choice) |
|---------|------|------------------|
| Space | O(ALPHABET_SIZE * N * M) | O(N * M) |
| Search | O(M) | O(M log N) |
| Best For | Small alphabet, many words | Large alphabet, space-constrained |
| Our Case | 88 tokens, full ASCII range | ✅ More efficient |

### 2. Why Min Heap for Top-K?

**Alternative Approaches:**
- **Full Sort:** O(n log n) - wasteful, we only need K items
- **Partial Sort:** O(n log k) using Max Heap - same complexity, but min heap simpler
- **Quickselect:** O(n) average but O(n²) worst case

**Our Min Heap Approach:**
- Time: O(n log k) where k=5 usually
- Space: O(k) - only store top K
- **Advantage:** Stable, predictable performance

### 3. Graph Representation Choice

**Why Adjacency List over Adjacency Matrix?**

| Aspect | Adjacency Matrix | Adjacency List (Our Choice) |
|--------|------------------|----------------------------|
| Space | O(V²) | O(V + E) |
| Check Edge | O(1) | O(log M) with map |
| Add Edge | O(1) | O(log M) |
| Our Case | 88² = 7,744 cells | Only actual edges stored ✅ |

**Our Implementation:**
```cpp
unordered_map<string, map<string, int>>
     ↓              ↓           ↓
  from_token    to_token    weight
```
- Outer map: O(1) lookup for source token
- Inner map: O(log m) for sorted neighbors (m = out-degree)
- Sparse graph optimization: Most tokens don't co-occur with all others

### 4. LRU Cache Implementation

**Why Hash Map + Doubly Linked List?**

```
HashMap: O(1) lookup
    ↓
Node in DLL ←→ Node ←→ Node
    ↑
MRU (head)             LRU (tail)
```

**Operations:**
- **get(key):** O(1) hash lookup + O(1) move to front
- **put(key, value):** O(1) insert at front + O(1) remove tail if full
- **Total:** O(1) for all operations!

**Alternative Rejected:**
- LinkedHashMap (not in C++ STL)
- Simple HashMap (no LRU eviction)
- Priority Queue (O(log n) updates)

### 5. Scoring Algorithm

**Formula:**
```
score = frequency_score + context_boost

where:
  frequency_score = freqStore.get(token)
  context_boost = graph.getBoost(lastToken, token) * weight_factor
```

**Example Calculation:**
```
Last token: "for"
Current candidate: "int"

frequency_score = freqStore.get("int") = 10
context_boost = graph.getBoost("for", "int") = 3  // co-occurred 3 times

final_score = 10 + 3 = 13
```

---

## 📊 Time Complexity Summary

| Operation | Data Structure | Time Complexity | Why |
|-----------|---------------|----------------|-----|
| Insert token | TST | O(m log n) | m=length, n=nodes at level |
| Prefix search | TST | O(m log n + k) | Find prefix + collect k results |
| Top-K ranking | Min Heap | O(n log k) | n candidates, k results |
| Frequency lookup | Hash Map | O(1) | Direct hashing |
| Frequency update | Hash Map | O(1) | Direct hashing |
| Cache lookup | LRU (HashMap) | O(1) | Hash-based |
| Cache update | LRU (DLL) | O(1) | Linked list ops |
| Add co-occurrence | Graph | O(log m) | Sorted adjacency list |
| Undo/Redo | Stack | O(1) | Stack push/pop |
| Substring search | KMP | O(n + m) | Linear scan |

---

## 🎓 Key Talking Points for Presentation

### Why These Data Structures?

1. **TST over Trie:** "We chose TST because it's 3x more space-efficient for our 88-token dictionary while maintaining fast O(m log n) searches."

2. **Min Heap for Ranking:** "Instead of sorting all candidates O(n log n), the min heap gives us top-K in O(n log k) time - much faster when K << n."

3. **Graph for Context:** "The directed weighted graph captures real user patterns. If users often type 'for' then 'int', the graph learns this and boosts 'int' contextually."

4. **Two Stacks for History:** "Classic two-stack approach: one for undo, one for redo. Simple, efficient, unlimited history."

5. **LRU Cache:** "Hybrid data structure combining hash map (O(1) lookup) with doubly-linked list (O(1) LRU eviction). Best of both worlds."

6. **Hash Maps Everywhere:** "Three hash maps serve different purposes: caching (LRU), frequency tracking, and graph adjacency. All provide O(1) operations."

### Real-World Applications

- **VSCode/IntelliJ:** Similar TST/Trie for code completion
- **Google Search:** Autocomplete uses trie-like structures
- **Text Editors:** Undo/redo stacks are universal
- **Recommendation Systems:** Graph-based co-occurrence (like Netflix)
- **Browser History:** LRU cache for recent pages

---

## 🎯 Presentation Flow (15-20 minutes)

### Slide 1-2: Introduction (2 min)
- Problem: Code completion needs to be fast and smart
- Solution: Intelligent autocomplete using 6 data structures

### Slide 3-4: System Overview (3 min)
- Architecture diagram
- Data flow from input to output
- 88-token C++ keyword dictionary

### Slide 5-10: Data Structures (8 min)
- 1 slide per data structure
- Show code snippet, explain purpose, complexity
- TST → MinHeap → Stack → Graph → HashMap → KMP

### Slide 11-12: Live Demo (5 min)
- Run 3-4 key demos from above
- Show TST search, heap ranking, stack undo/redo, graph context

### Slide 13: Complexity Analysis (2 min)
- Show time complexity table
- Highlight O(1) and O(log n) operations

### Slide 14: Conclusion (1 min)
- Recap: 6 data structures working together
- Real-world applications
- Future enhancements (ML-based scoring, bigger dictionaries)

---

## 💡 Advanced Features to Highlight

1. **Adaptive Learning:** System improves with usage (frequency + graph)
2. **Context Awareness:** Suggestions based on previous tokens (graph)
3. **Efficient Caching:** LRU prevents redundant computations
4. **User Control:** Manual frequency bumps, undo/redo, toggle features
5. **Scalability:** All operations O(1) or O(log n) - handles large dictionaries

---

## 🚀 Future Enhancements (Optional Slide)

1. **Multi-level Trie:** Support nested namespaces (std::vector)
2. **Fuzzy Matching:** Levenshtein distance for typo tolerance
3. **Machine Learning:** Train on real codebases for better scoring
4. **Persistent Storage:** Save graph and frequencies to disk
5. **Multi-user Learning:** Aggregate patterns across users

---

## ✅ Checklist Before Presentation

- [ ] Code compiles without errors
- [ ] All 7 demos work smoothly
- [ ] Can explain each data structure's time complexity
- [ ] Understand TST vs Trie trade-offs
- [ ] Can draw graph, heap, stack on whiteboard
- [ ] Know real-world examples for each structure
- [ ] Prepared to answer "why not X?" questions
- [ ] Have backup slides for deep technical questions

---

**Good Luck with Your Presentation! 🎉**
