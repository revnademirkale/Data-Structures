#ifndef MPQ_H
#define MPQ_H

#include <vector>
using namespace std;

// This struct keeps the value and label together.
// In homework document, they say each item has 2 parts:
// the height (value) and the id (label).
struct MPQItem {
    int value;
    int label;
};

class MPQ {
private:
    // This will be our heap. We start index from 1.
    vector<MPQItem> Heap;

    // Location array: for each label, we store the index in heap.
    // Homework text say this is important so we can remove by label fast.
    vector<int> Location;

    int currentSize;  // how many items in heap now

    // Swap two nodes in heap and also fix their Location values.
    // I try make it simple and not too long.
    void swapNodes(int i, int j) {
        MPQItem temp = Heap[i];
        Heap[i] = Heap[j];
        Heap[j] = temp;

        // update locations also
        Location[Heap[i].label] = i;
        Location[Heap[j].label] = j;
    }

    // This function move item up if it is bigger than parent.
    // (because we want max-heap)
    void percolateUp(int index) {
        while (index > 1) {
            int parent = index / 2;

            // if parent is already bigger, stop
            if (Heap[parent].value >= Heap[index].value)
                break;

            // swap with parent
            swapNodes(parent, index);
            index = parent;
        }
    }

    // This moves item down if one of the childs is bigger.
    void percolateDown(int index) {
        while (2 * index <= currentSize) {
            int child = 2 * index;

            // choose the bigger child
            if (child + 1 <= currentSize &&
                Heap[child + 1].value > Heap[child].value)
                child++;

            // if parent already bigger, stop
            if (Heap[index].value >= Heap[child].value)
                break;

            swapNodes(index, child);
            index = child;
        }
    }

public:
    // Constructor: we make space for heap and location.
    // Homework document say we need location array same size with labels.
    MPQ(int maxLabels) {
        Heap.resize(maxLabels + 1);
        Location.resize(maxLabels + 1, -1);
        currentSize = 0;
    }

    // Destructor (nothing special here)
    ~MPQ() {}

    // Check if heap empty
    bool IsEmpty() const {
        return currentSize == 0;
    }

    // Insert a new value with its label.
    // We put it at bottom and move up.
    void insert(int value, int label) {
        currentSize++;
        Heap[currentSize].value = value;
        Heap[currentSize].label = label;
        Location[label] = currentSize;

        // fix heap property
        percolateUp(currentSize);
    }

    // Remove element by label.
    // This is the special part of MPQ, normal heap can't do this fast.
    int Remove(int label) {
        int index = Location[label];

        // if already removed
        if (index == -1) return 0;

        int removedValue = Heap[index].value;
        Location[label] = -1;  // delete from location table

        // if it is last element, just reduce size
        if (index == currentSize) {
            currentSize--;
            return removedValue;
        }

        // move last to this place
        swapNodes(index, currentSize);
        currentSize--;

        // fix heap (maybe go up or down)
        percolateUp(index);
        percolateDown(index);

        return removedValue;
    }

    // Return max value in heap.
    // In max-heap, it is always position 1.
    int GetMax() const {
        if (currentSize == 0) return 0;
        return Heap[1].value;
    }
};

#endif
