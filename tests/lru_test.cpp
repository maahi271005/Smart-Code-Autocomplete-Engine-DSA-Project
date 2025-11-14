#include <iostream>
#include <cassert>
#include "../include/lru.h"

void testLRU() {
    lru_cache cache(3);
    
    cache.put("a", {"apple", "apricot"});
    cache.put("b", {"banana", "berry"});
    cache.put("c", {"cherry", "coconut"});
    
    assert(cache.exists("a"));
    assert(cache.exists("b"));
    assert(cache.exists("c"));
    
    auto result = cache.get("a");
    assert(result.size() == 2);
    assert(result[0] == "apple");
    
    std::cout << "LRU Basic Operations tests passed" << std::endl;
}

void testLRUEviction() {
    lru_cache cache(2);
    
    cache.put("a", {"apple"});
    cache.put("b", {"banana"});
    cache.put("c", {"cherry"});
    
    assert(!cache.exists("a"));
    assert(cache.exists("b"));
    assert(cache.exists("c"));
    
    std::cout << "LRU Eviction tests passed" << std::endl;
}

void testLRUUpdate() {
    lru_cache cache(3);
    
    cache.put("a", {"apple"});
    cache.put("b", {"banana"});
    cache.put("c", {"cherry"});
    
    cache.get("a"); 
    
    cache.put("d", {"date"}); 
    
    assert(cache.exists("a"));  
    assert(!cache.exists("b")); 
    assert(cache.exists("c"));
    assert(cache.exists("d"));
    
    std::cout << "LRU Update tests passed" << std::endl;
}

void testLRUEmpty() {
    lru_cache cache(5);
    
    auto result = cache.get("nonexistent");
    assert(result.empty());
    assert(!cache.exists("test"));
    
    std::cout << "LRU Empty tests passed" << std::endl;
}

int main() {
    std::cout << "\nRunning LRU Cache Tests...\n" << std::endl;
    
    testLRU();
    testLRUEviction();
    testLRUUpdate();
    testLRUEmpty();
    
    std::cout << "\n All LRU Cache tests passed!\n" << std::endl;
    
    return 0;
}
