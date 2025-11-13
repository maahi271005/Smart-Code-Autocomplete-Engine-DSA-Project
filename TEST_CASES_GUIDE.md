# Test Cases Guide - Smart Code Autocomplete Engine

## 📋 Table of Contents
1. [Quick Reference](#quick-reference)
2. [Detailed Test Cases](#detailed-test-cases)
3. [Demo Scripts](#demo-scripts)
4. [Expected Outputs](#expected-outputs)

---

## 🎯 Quick Reference

| Data Structure | Test Case | Command | What It Shows |
|---------------|-----------|---------|---------------|
| **TST** | Prefix Search | Type `for` | Fast prefix matching |
| **Min Heap** | Top-K Selection | Type `p` | Ranked top 5 results |
| **Stack** | Undo/Redo | `:undo`, `:redo` | History management |
| **Graph** | Context Learning | Accept tokens, `:graph` | Co-occurrence patterns |
| **HashMap (Freq)** | Learning | `:bump int`, type `i` | Frequency tracking |
| **HashMap (Cache)** | LRU Cache | Type `for` twice | Faster 2nd time |
| **KMP** | Substring Search | `:toggle_contains`, type `main` | Pattern matching |

---

## 📝 Detailed Test Cases

### Test Case 1: TST - Prefix Search
**Data Structure:** Ternary Search Tree
**File:** `tst.h`, `tst.cpp`

#### Test 1.1: Basic Prefix Match
**Input:**
```
for
```

**Expected Output:**
```
Suggestions:
  1. for (score=0)
```

**What to Explain:**
- TST traverses tree comparing characters
- Finds node where prefix ends
- Collects all words under that node
- Complexity: O(m log n) where m = prefix length

#### Test 1.2: Multiple Matches
**Input:**
```
p
```

**Expected Output:**
```
Suggestions:
  1. printf (score=0)
  2. private (score=0)
  3. protected (score=0)
  4. public (score=0)
  5. push_back (score=0)
```

**What to Explain:**
- TST finds multiple words efficiently
- All share common prefix 'p'
- Results collected via in-order traversal

#### Test 1.3: No Match
**Input:**
```
xyz
```

**Expected Output:**
```
No suggestions found for 'xyz'
```

**What to Explain:**
- TST traversal returns null when path not found
- No unnecessary computation on non-existent prefixes

#### Test 1.4: Complete Word Search
**Input:**
```
vector
```

**Expected Output:**
```
Suggestions:
  1. vector (score=0)
```

**What to Explain:**
- Exact matches also work
- isEndOfString flag marks valid words

---

### Test Case 2: Min Heap - Top-K Ranking
**Data Structure:** Min Heap (Priority Queue)
**File:** `minheap.h`, `minheap.cpp`, `ranker.cpp`

#### Test 2.1: Ranking with Scores
**Setup:**
```
:bump for 10
:bump foreach 5
:bump format 3
```

**Input:**
```
for
```

**Expected Output:**
```
Suggestions:
  1. for (score=10)
  2. foreach (score=5)
  3. format (score=3)
```

**What to Explain:**
- Min heap maintains top K=5 items
- Scores determine ranking
- Heap keeps minimum at root
- When heap is full, compares new score with min
- Only replaces if new score is higher

#### Test 2.2: More Candidates Than K
**Setup:**
```
:bump printf 20
:bump println 15
:bump print 10
:bump private 8
:bump protected 6
:bump public 4
:bump push_back 2
```

**Input:**
```
p
```

**Expected Output:**
```
Suggestions:
  1. printf (score=20)
  2. println (score=15)
  3. print (score=10)
  4. private (score=8)
  5. protected (score=6)
(public and push_back excluded - only top 5 shown)
```

**What to Explain:**
- Heap size limited to K=5
- Efficiently filters 7 candidates to top 5
- O(n log k) complexity instead of O(n log n) for full sort
- Memory efficient: only stores K items

#### Test 2.3: Heap Property Verification
**Visual Representation:**
```
After inserting: 20, 15, 10, 8, 6, 4, 2 with K=5

Step-by-step heap construction:
Insert 20: [20]
Insert 15: [15, 20]
Insert 10: [10, 20, 15]
Insert 8:  [8, 10, 15, 20]
Insert 6:  [6, 8, 15, 20, 10]  (full, K=5)
Insert 4:  [6, 8, 15, 20, 10]  (4 < 6, rejected)
Insert 2:  [6, 8, 15, 20, 10]  (2 < 6, rejected)

Final sorted output: [20, 15, 10, 8, 6]
```

---

### Test Case 3: Stack - Undo/Redo
**Data Structure:** Stack (Two Stacks)
**File:** `stack.h`, `stack.cpp`

#### Test 3.1: Basic Undo
**Input:**
```
for
1
int
1
:undo
```

**Expected Output:**
```
> for
Suggestions:
  1. for (score=0)
Accept by number or token: 1
Accepted: for

> int
Suggestions:
  1. int (score=0)
Accept by number or token: 1
Accepted: int

> :undo
Undo: Removed 'int'
```

**Stack State:**
```
After "for":   undoStack: ["for"]              redoStack: []
After "int":   undoStack: ["for", "int"]       redoStack: []
After undo:    undoStack: ["for"]              redoStack: ["int"]
```

#### Test 3.2: Multiple Undo/Redo
**Input:**
```
for
1
int
1
auto
1
:undo
:undo
:redo
:redo
```

**Expected Output:**
```
Accepted: for
Accepted: int
Accepted: auto
Undo: Removed 'auto'
Undo: Removed 'int'
Redo: Restored 'int'
Redo: Restored 'auto'
```

**Stack States:**
```
Initial:         undoStack: []                        redoStack: []
After for:       undoStack: ["for"]                   redoStack: []
After int:       undoStack: ["for", "int"]            redoStack: []
After auto:      undoStack: ["for", "int", "auto"]    redoStack: []
After undo:      undoStack: ["for", "int"]            redoStack: ["auto"]
After undo:      undoStack: ["for"]                   redoStack: ["auto", "int"]
After redo:      undoStack: ["for", "int"]            redoStack: ["auto"]
After redo:      undoStack: ["for", "int", "auto"]    redoStack: []
```

#### Test 3.3: Undo Empty Stack
**Input:**
```
:undo
```

**Expected Output:**
```
Nothing to undo
```

**What to Explain:**
- Stack checks if empty before popping
- Prevents underflow errors

#### Test 3.4: Redo After New Action
**Input:**
```
for
1
int
1
:undo
auto
1
:redo
```

**Expected Output:**
```
Accepted: for
Accepted: int
Undo: Removed 'int'
Accepted: auto
Nothing to redo
```

**Stack States:**
```
After int:        undoStack: ["for", "int"]     redoStack: []
After undo:       undoStack: ["for"]            redoStack: ["int"]
After auto:       undoStack: ["for", "auto"]    redoStack: []  ← Cleared!
After redo:       undoStack: ["for", "auto"]    redoStack: []  ← Empty
```

**What to Explain:**
- New action clears redo stack
- Standard undo/redo behavior (like text editors)

---

### Test Case 4: Graph - Co-occurrence Learning
**Data Structure:** Directed Weighted Graph
**File:** `graph.h`, `graph.cpp`

#### Test 4.1: Building Graph Edges
**Input:**
```
for
1
int
1
for
1
int
1
for
1
char
1
:graph
```

**Expected Output:**
```
Accepted: for
Accepted: int
Accepted: for
Accepted: int
Accepted: for
Accepted: char

=== Co-occurrence Graph ===
for → int(2) char(1)
int → for(2)
```

**Graph Visualization:**
```
    ┌─────┐ 2 ┌─────┐
    │ for │──→│ int │
    └─────┘   └─────┘
       │ ↑
       │ └─────2
       │
       │ 1
       ↓
    ┌──────┐
    │ char │
    └──────┘
```

**What to Explain:**
- Directed edges: "for"→"int" means "for" followed by "int"
- Weights: Number of times co-occurrence observed
- Adjacency list representation: unordered_map<string, map<string, int>>

#### Test 4.2: Context-Aware Suggestions
**Setup: Build context**
```
for
1
int
1
for
1
int
1
for
1
int
1
```

**Now test:**
```
for
1
i
```

**Expected Output:**
```
> for
Suggestions:
  1. for (score=3)
Accept: 1

> i
Suggestions:
  1. int (score=6)    ← Higher score due to graph boost!
  2. inline (score=0)
  3. if (score=0)
  ...
```

**Score Calculation:**
```
For "int" after "for":
  frequency_score = 3 (accepted 3 times)
  graph_boost = 3 (for→int edge weight)
  total_score = 3 + 3 = 6

For "inline" after "for":
  frequency_score = 0 (never used)
  graph_boost = 0 (no for→inline edge)
  total_score = 0
```

**What to Explain:**
- Context matters: same prefix, different previous token = different scores
- Graph edges provide contextual boost
- Implements basic N-gram (bigram) model

#### Test 4.3: Complex Graph Patterns
**Input:**
```
if
1
true
1
if
1
false
1
if
1
x
1
:graph
```

**Expected Output:**
```
=== Co-occurrence Graph ===
if → true(1) false(1) x(1)
true → if(1)
false → if(1)
x → if(1)
```

**What to Explain:**
- One source node (if) with multiple targets
- Models real coding patterns (if followed by conditions)
- Graph can grow to model complex sequences

---

### Test Case 5: HashMap - Frequency Store
**Data Structure:** Hash Map (unordered_map)
**File:** `freq_store.h`, `freq_store.cpp`

#### Test 5.1: Frequency Tracking
**Input:**
```
for
1
for
1
for
1
int
1
int
1
```

**Check frequencies:**
```
# (manually inspect or query)
"for": frequency = 3
"int": frequency = 2
```

**What to Explain:**
- Hash map provides O(1) lookups
- Each acceptance increments frequency
- Frequencies persist during session

#### Test 5.2: Manual Frequency Bump
**Input:**
```
:bump vector 10
vec
```

**Expected Output:**
```
Bumped frequency of 'vector' by 10

> vec
Suggestions:
  1. vector (score=10)
```

**What to Explain:**
- :bump command adds to frequency manually
- Simulates heavy usage quickly
- Hash map update is O(1)

#### Test 5.3: Frequency Affects Ranking
**Setup:**
```
:bump printf 20
:bump print 5
:bump private 15
```

**Input:**
```
pri
```

**Expected Output:**
```
Suggestions:
  1. printf (score=20)
  2. private (score=15)
  3. print (score=5)
```

**What to Explain:**
- Higher frequency → higher score → better rank
- Simple but effective learning mechanism

---

### Test Case 6: HashMap - LRU Cache
**Data Structure:** Hash Map + Doubly Linked List
**File:** `lru.h`, `lru.cpp`

#### Test 6.1: Cache Hit vs Miss
**First Query (Cache Miss):**
```
> for
(TST search takes time)
Suggestions:
  1. for (score=0)
```

**Second Query (Cache Hit):**
```
> for
(Instant from cache)
Suggestions:
  1. for (score=0)
```

**What to Explain:**
- First query: TST traversal + result stored in cache
- Second query: Direct cache lookup (O(1) via hash map)
- Cache key = prefix string ("for")
- Cache value = list of results

#### Test 6.2: LRU Eviction (Conceptual)
**Scenario:** Cache capacity = 50

**Sequence:**
```
Query "for"  → Cache: ["for"]
Query "int"  → Cache: ["for", "int"]
Query "auto" → Cache: ["for", "int", "auto"]
...
Query 51st unique prefix → Evicts "for" (least recently used)
```

**LRU Order (DLL):**
```
MRU → [ "51st" ] ↔ [ "50th" ] ↔ ... ↔ [ "int" ] ↔ [ "auto" ] → LRU
       (head)                                              (tail)
```

**What to Explain:**
- Doubly-linked list maintains recency order
- On access: Move node to front (MRU)
- On insert when full: Remove tail (LRU)
- Hash map provides O(1) node lookup

#### Test 6.3: Cache Validation
**Input:**
```
for
(skip)
int
(skip)
auto
(skip)
for
(skip)
```

**Cache State:**
```
After "for":  Cache: ["for"]
After "int":  Cache: ["for", "int"]
After "auto": Cache: ["for", "int", "auto"]
After "for":  Cache: ["int", "auto", "for"]  ← "for" moved to front (MRU)
```

---

### Test Case 7: KMP - Substring Search
**Data Structure:** String Matching Algorithm
**File:** `kmp.h`, `kmp.cpp`

#### Test 7.1: Enable Substring Search
**Input:**
```
:toggle_contains
main
```

**Expected Output:**
```
Substring search: ON

> main
Suggestions:
  1. main (score=0)
(potentially more if "main" appears as substring in other words)
```

**What to Explain:**
- Without KMP: Only prefix matches (TST)
- With KMP: Substring matches anywhere in word
- KMP preprocesses pattern, builds LPS array
- Linear time O(n+m) scan

#### Test 7.2: KMP Pattern Matching
**Scenario:** Word list includes "domain", "main", "remain", "maintenance"

**Input:**
```
:toggle_contains
main
```

**Expected Output:**
```
Suggestions:
  1. main (score=0)
  2. domain (score=0)      ← "main" at end
  3. remain (score=0)      ← "main" at end
  4. maintenance (score=0) ← "main" at start
```

**KMP Process:**
```
Word: "domain"
Pattern: "main"
LPS: [0, 0, 0, 0]

Scanning:
d o m a i n
    m a i n  ← Match found at position 2
```

**What to Explain:**
- KMP finds pattern in O(n+m) time
- LPS array avoids re-checking characters
- Alternative (naive): O(n*m) time

#### Test 7.3: LPS Array Explanation
**Pattern:** "aba"

**LPS Computation:**
```
Index: 0  1  2
Char:  a  b  a
LPS:   0  0  1

Explanation:
- LPS[0] = 0 (single char, no prefix-suffix match)
- LPS[1] = 0 ("ab", no proper prefix equals suffix)
- LPS[2] = 1 ("aba", "a" is both prefix and suffix)
```

**Pattern:** "ababc"

**LPS Computation:**
```
Index: 0  1  2  3  4
Char:  a  b  a  b  c
LPS:   0  0  1  2  0

Explanation:
- LPS[3] = 2 ("abab", "ab" is both prefix and suffix)
- LPS[4] = 0 ("ababc", no prefix equals suffix)
```

---

## 🎬 Demo Scripts (Copy-Paste Ready)

### Demo Script 1: Complete TST Demo (2 minutes)
```bash
# Start program
./smart_autocomplete

# Test 1: Single match
for
1

# Test 2: Multiple matches
p


# Test 3: No match
xyz


# Test 4: Longer prefix
vec
1

# Exit
:exit
```

**Expected Flow:**
```
> for
Suggestions: 1. for (score=1)
Accepted: for

> p
Suggestions: printf, private, protected, public, push_back

> xyz
No suggestions found for 'xyz'

> vec
Suggestions: 1. vector (score=0)
Accepted: vector

> :exit
Goodbye!
```

---

### Demo Script 2: Min Heap Ranking Demo (3 minutes)
```bash
./smart_autocomplete

# Build frequencies
:bump printf 25
:bump print 20
:bump println 15
:bump private 10
:bump protected 5
:bump public 3
:bump push_back 1

# Query to see ranking
p


:exit
```

**Expected Output:**
```
> p
Suggestions:
  1. printf (score=25)
  2. print (score=20)
  3. println (score=15)
  4. private (score=10)
  5. protected (score=5)
(public and push_back not shown - heap keeps top 5)
```

---

### Demo Script 3: Stack Undo/Redo Demo (2 minutes)
```bash
./smart_autocomplete

# Accept several tokens
for
1
int
1
auto
1

# Undo operations
:undo
:undo
:undo

# Redo operations
:redo
:redo

# New action clears redo stack
double
1

# Try redo (should fail)
:redo

:exit
```

**Expected Output:**
```
Accepted: for
Accepted: int
Accepted: auto
Undo: Removed 'auto'
Undo: Removed 'int'
Undo: Removed 'for'
Redo: Restored 'for'
Redo: Restored 'int'
Accepted: double
Nothing to redo  ← Redo stack was cleared
```

---

### Demo Script 4: Graph Context Demo (4 minutes)
```bash
./smart_autocomplete

# Build co-occurrence pattern: for → int
for
1
int
1

# Repeat to strengthen edge
for
1
int
1

# Build another pattern: for → char
for
1
char
1

# View graph
:graph

# Test context boost
for
1
i


:exit
```

**Expected Output:**
```
Accepted: for
Accepted: int
Accepted: for
Accepted: int
Accepted: for
Accepted: char

=== Co-occurrence Graph ===
for → int(2) char(1)
int → for(2)
char → for(1)

> for
Accepted: for

> i
Suggestions:
  1. int (score=2)     ← Gets boost from for→int edge
  2. if (score=0)
  3. inline (score=0)
```

---

### Demo Script 5: Frequency Learning Demo (2 minutes)
```bash
./smart_autocomplete

# Accept same token multiple times
int
1
int
1
int
1

# Query with prefix
i


# Manually boost another token
:bump inline 10

# Query again
i


:exit
```

**Expected Output:**
```
Accepted: int
Accepted: int
Accepted: int

> i
Suggestions:
  1. int (score=3)
  2. if (score=0)
  3. inline (score=0)

Bumped frequency of 'inline' by 10

> i
Suggestions:
  1. inline (score=10)
  2. int (score=3)
  3. if (score=0)
```

---

### Demo Script 6: KMP Substring Demo (2 minutes)
```bash
./smart_autocomplete

# Normal mode (prefix only)
main


# Enable substring search
:toggle_contains

# Now substring matches work
main


# Disable and compare
:toggle_contains
main


:exit
```

**Expected Output:**
```
> main
Suggestions:
  1. main (score=0)

Substring search: ON

> main
Suggestions:
  1. main (score=0)
  (potentially: domain, remain if in dictionary)

Substring search: OFF

> main
Suggestions:
  1. main (score=0)
```

---

## 📊 Expected Output Reference

### Complete Session Example
```bash
$ ./smart_autocomplete

Loaded 88 tokens from seed file.

=== Smart Autocomplete Engine ===
Type ':help' for commands
Type ':exit' or ':q' to quit

> for

Suggestions:
  1. for (score=0)

Accept by number or token (or press Enter to skip): 1
Accepted: for

> int

Suggestions:
  1. int (score=0)

Accept by number or token (or press Enter to skip): 1
Accepted: int

> :graph

=== Co-occurrence Graph ===
for → int(1)

> :undo
Undo: Removed 'int'

> :redo
Redo: Restored 'int'

> :bump for 5
Bumped frequency of 'for' by 5

> for

Suggestions:
  1. for (score=5)

Accept by number or token (or press Enter to skip):

> :exit
Goodbye!
```

---

## ✅ Pre-Demo Checklist

### Before Each Demo:
- [ ] Program compiles: `make clean && make`
- [ ] Start fresh: Exit and restart program between major demos
- [ ] Have demo script ready to copy-paste
- [ ] Know what output to expect
- [ ] Can explain the data structure involved
- [ ] Understand time complexity of operations

### Technical Prep:
- [ ] Can explain TST vs Trie trade-off
- [ ] Can draw min heap on whiteboard
- [ ] Understand stack push/pop operations
- [ ] Can sketch graph structure
- [ ] Know hash map collision handling (though not shown)
- [ ] Understand KMP LPS array construction

---

## 🎯 Recommended Demo Order for Presentation

1. **TST (2 min):** Basic prefix search - shows core functionality
2. **Min Heap (2 min):** Ranking with bumped frequencies - shows intelligence
3. **Graph (3 min):** Context learning - most impressive feature
4. **Stack (2 min):** Undo/redo - easy to understand, relatable
5. **HashMap (1 min):** Quick mention of frequency tracking
6. **KMP (1 min):** Optional - if time permits

**Total:** ~11 minutes demo, leaves 4-9 min for slides and Q&A

---

## 💡 Pro Tips for Demo

1. **Use :bump extensively:** Don't wait for natural frequency buildup
2. **Prepare data:** Create a startup script that pre-loads frequencies
3. **Keep graph simple:** Don't create too many edges, hard to visualize
4. **Demonstrate failures:** Show "no suggestions" and "nothing to undo"
5. **Compare before/after:** Show impact of frequency/graph boosts
6. **Narrate actions:** Explain what you're typing as you type
7. **Have backup:** Pre-record demo video in case of live issues

---

## 🚨 Common Issues & Solutions

### Issue 1: No Suggestions
**Cause:** TST doesn't have the word
**Solution:** Only use words from data/words.txt

### Issue 2: All Scores Are 0
**Cause:** No frequency data yet
**Solution:** Use :bump command or accept tokens multiple times

### Issue 3: Graph Empty
**Cause:** Need to accept multiple tokens in sequence
**Solution:** Accept at least 2 tokens before :graph

### Issue 4: Cache Not Obvious
**Cause:** Speed difference not noticeable
**Solution:** Mention it verbally, not critical to see

---

## 📝 Final Notes

- **Practice:** Run through demos 2-3 times before presentation
- **Time Management:** Each demo should take 1-3 minutes
- **Flexibility:** Skip KMP/Cache demos if running short on time
- **Focus:** TST, MinHeap, Graph, Stack are the stars - prioritize these
- **Confidence:** Know your code, explain trade-offs, show complexity analysis

**You've got this! 🚀**
