#ifndef __PRIORITY_QUEUE__
#define __PRIORITY_QUEUE__

#include <vector>

// Index calculations
#define parent(i)    ((i) / 2)
#define leftChild(i) ((i) * 2)

/**
 * class T must have: (i) accessible field int queueIndex; (ii) operator< defined.
 */

template <class T>
class PriorityQueue {
  public:
    PriorityQueue();

    void insert(T *x);
    T   *extractMin();
    void decreaseKey(T *x);
    bool empty();

  protected:
    std::vector<T *> _heap;

    void        heapifyUp(unsigned i);
    void        heapifyDown(unsigned i);
    inline void set(unsigned i, T *x);
};

template <class T>
PriorityQueue<T>::PriorityQueue() {
    // Indices will be used starting in 1
    // To facilitate parent/child calculations
    this->_heap.push_back(nullptr);
}

template <class T>
bool PriorityQueue<T>::empty() {
    return this->_heap.size() == 1;
}

template <class T>
T *PriorityQueue<T>::extractMin() {
    T *x     = _heap[1];
    _heap[1] = _heap.back();
    _heap.pop_back();
    if (_heap.size() > 1) heapifyDown(1);
    x->queueIndex = 0;
    return x;
}

template <class T>
void PriorityQueue<T>::insert(T *x) {
    _heap.push_back(x);
    heapifyUp(_heap.size() - 1);
}

template <class T>
void PriorityQueue<T>::decreaseKey(T *x) {
    heapifyUp(x->queueIndex);
}

template <class T>
void PriorityQueue<T>::heapifyUp(unsigned i) {
    T *x = _heap[i];
    while (i > 1 && *x < *_heap[parent(i)]) {
        set(i, _heap[parent(i)]);
        i = parent(i);
    }
    set(i, x);
}

template <class T>
void PriorityQueue<T>::heapifyDown(unsigned i) {
    auto x = _heap[i];
    while (true) {
        unsigned k = leftChild(i);
        if (k >= _heap.size()) break;
        if (k + 1 < _heap.size() && *_heap[k + 1] < *_heap[k]) ++k; // right child of i
        if (!(*_heap[k] < *x)) break;
        set(i, _heap[k]);
        i = k;
    }
    set(i, x);
}

template <class T>
void PriorityQueue<T>::set(unsigned i, T *x) {
    _heap[i]      = x;
    x->queueIndex = i;
}

#endif /* __PRIORITY_QUEUE__ */
