#include <iostream>
#include <cassert>
#include "../include/minheap.h"
using namespace std;

void test1(){
    MinHeap heap(5);
    heap.insert(3.0,"three");
    heap.insert(1.0,"one");
    heap.insert(5.0,"five");
    heap.insert(2.0,"two");
    assert(heap.size()==4);
    auto minele=heap.getMin();
    assert(minele.first==1.0);
    assert(minele.second=="one");
    cout<<"Heap insert and extract tests passed"<<endl;
}

void test2() {
    MinHeap heap(3);
    heap.insert(1.0,"one");
    heap.insert(2.0,"two");
    heap.insert(3.0,"three");
    assert(heap.size()==3);
  
    heap.insert(4.0,"four");
    assert(heap.size()==3);
    auto all=heap.getAll();
    assert(all.size()==3);
    assert(all[0].first==4.0);
    cout << "Heap fixed size tests passed"<<endl;
}

void test3() {
    MinHeap heap(5);

    heap.insert(5.0,"five");
    heap.insert(3.0,"three");
    heap.insert(7.0,"seven");
    heap.insert(1.0,"one");

    auto all=heap.getAll();

    assert(all.size()==4);
    assert(all[0].first>=all[1].first);
    assert(all[1].first>=all[2].first);
    assert(all[2].first>=all[3].first);
    cout << "Heap getall tests passed"<<endl;
}
void test4() {
    MinHeap heap(5);
    assert(heap.isEmpty());
    assert(heap.size()==0);
    heap.insert(1.0,"one");
    assert(!heap.isEmpty());
    cout << "Heap empty tests passed"<<endl;
}

int main(){
    cout<<"Running minheap tests..."<<endl;
    test1();
    test2();
    test3();
    test4();
    cout<<"All minheap tests passed!"<<endl;
    return 0;
}

