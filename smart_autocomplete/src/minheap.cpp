#include "../include/minheap.h"
#include <stdexcept>
#include <algorithm>

MinHeap::MinHeap(int k) : maxSize(k) {}

void MinHeap::heapifyUp(int index) {
    while (index>0 && heap[parent(index)].first > heap[index].first) {
        std::swap(heap[parent(index)], heap[index]);
        index = parent(index);
    }
}

void MinHeap::heapifyDown(int index){
    int smallest = index;
    int left =leftChild(index);
    int right =rightChild(index);
    
    if (left< heap.size() && heap[left].first < heap[smallest].first) {
        smallest = left;
    }
    
    if (right < heap.size() && heap[right].first < heap[smallest].first) {
        smallest = right;
    }
    
    if (smallest!= index) {
        std::swap(heap[index], heap[smallest]);
        heapifyDown(smallest);
    }
}
void MinHeap::insert(double score, const std::string& word) {
    if (heap.size()<maxSize) {
        heap.push_back({score, word});
        heapifyUp(heap.size() - 1);
    }
    else if (score > heap[0].first){
        heap[0] = {score, word};
        heapifyDown(0);
    }
}

std::pair<double, std::string> MinHeap::extractMin() {
    if (heap.empty()) {
        throw std::runtime_error("Heap is empty");
    }
    
    auto minElement = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    
    if (!heap.empty()) {
        heapifyDown(0);
    }
    
    return minElement;
}

std::pair<double, std::string> MinHeap::getMin() {
    if (heap.empty()) {
        throw std::runtime_error("Heap is empty");
    }
    return heap[0];
}

std::vector<std::pair<double, std::string>> MinHeap::getAll(){
    std::vector<std::pair<double, std::string>> result = heap;
    std::sort(result.begin(), result.end(), 
            [](const auto& a, const auto& b){
                return a.first > b.first; 
            });
    return result;
}

void MinHeap::clear() {
    heap.clear();
}


