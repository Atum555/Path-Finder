#ifndef __PRIORITY_QUEUE__
#define __PRIORITY_QUEUE__

#include <vector>

// Index calculations
#define parent(i)    ((i) / 2)
#define leftChild(i) ((i) * 2)

/**
 * class T must have: (i) accessible field int _queueIndex; (ii) operator< defined.
 */



/**
 * @class PriorityQueue
 * @brief A min-heap based priority queue for elements of type T.
 *
 * This class manages a collection of pointers to elements of type T, maintaining
 * the heap property such that the element with the minimum key is always at the root.
 * It supports insertion, extraction of the minimum element, and decreasing the key
 * of an existing element.
 *
 * @note The type T is expected to support comparison operations required to maintain
 *       the heap order.
 *
 * Public Member Functions:
 * - PriorityQueue()
 *   @brief Constructs an empty priority queue.
 *
 * - void insert(T *x)
 *   @brief Inserts the pointer x into the priority queue.
 *   @param x A pointer to the element to be inserted.
 *
 * - T *extractMin()
 *   @brief Removes and returns the pointer to the element with the minimum key.
 *   @return Pointer to the element with the minimum key.
 *
 * - void decreaseKey(T *x)
 *   @brief Decreases the key value of the element pointed to by x and updates the heap.
 *   @param x A pointer to the element whose key is to be decreased.
 *
 * - bool empty()
 *   @brief Checks whether the priority queue is empty.
 *   @return True if the priority queue is empty; otherwise, false.
 *
 * Protected Member Functions:
 * - void heapifyUp(unsigned i)
 *   @brief Restores the heap property by moving the element at index i upward.
 *   @param i The index from which to start the heapify process.
 *
 * - void heapifyDown(unsigned i)
 *   @brief Restores the heap property by moving the element at index i downward.
 *   @param i The index from which to start the heapify process.
 *
 * - inline void set(unsigned i, T *x)
 *   @brief Sets the element at index i of the heap to the pointer x.
 *   @param i The index in the heap.
 *   @param x The pointer to the element to set at the specified index.
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
    x->_queueIndex = 0;
    return x;
}

template <class T>
void PriorityQueue<T>::insert(T *x) {
    _heap.push_back(x);
    heapifyUp(_heap.size() - 1);
}

template <class T>
void PriorityQueue<T>::decreaseKey(T *x) {
    heapifyUp(x->_queueIndex);
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
    x->_queueIndex = i;
}

#endif /* __PRIORITY_QUEUE__ */
