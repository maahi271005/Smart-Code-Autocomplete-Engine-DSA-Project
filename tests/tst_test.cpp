#include <iostream>
#include <cassert>
#include "../include/tst.h"

void testTSTInsertAndSearch() {
    TST tst;
    
    tst.insert("hello");
    tst.insert("world");
    tst.insert("help");
    tst.insert("hell");
    
    assert(tst.search("hello")==true);
    assert(tst.search("world")==true);
    assert(tst.search("help")==true);
    assert(tst.search("hell")==true);
    assert(tst.search("he")==false);
    assert(tst.search("worlds")==false);
    
    std::cout << "✓ TST Insert and Search tests passed" << std::endl;
}

void testTSTPrefixSearch() {
    TST tst;
    
    tst.insert("print");
    tst.insert("printf");
    tst.insert("println");
    tst.insert("private");
    tst.insert("protected");
    
    auto results = tst.prefixSearch("pri", 10);
    
    assert(results.size() >= 3);
    bool foundPrint = false;
    bool foundPrintf = false;
    bool foundPrivate = false;
    

    for (const auto& word : results) {
        if (word=="print"){
            foundPrint = true;
        }
        if (word=="printf"){
            foundPrintf = true;
        }
        if (word=="private"){
            foundPrivate = true;
        }
    }
    
    assert(foundPrint && foundPrintf && foundPrivate);
    
    std::cout << "✓ TST Prefix Search tests passed" << std::endl;
}

void testTSTEmptyCases(){
    TST tst;
    
    assert(tst.search("") == false);
    auto results = tst.prefixSearch("test", 5);
    assert(results.empty());
    
    tst.insert("test");
    assert(tst.search("test") == true);
    
    std::cout << "✓ TST Empty Cases tests passed" << std::endl;
}

int main() {
    std::cout << "\nRunning TST Tests...\n" << std::endl;
    
    testTSTInsertAndSearch();
    testTSTPrefixSearch();
    testTSTEmptyCases();
    
    std::cout << "\n All TST tests passed!\n" << std::endl;
    
    return 0;
}

