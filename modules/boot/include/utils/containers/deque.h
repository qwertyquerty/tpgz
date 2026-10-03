#ifndef TPGZ_BOOT_UTILS_DEQUE_H
#define TPGZ_BOOT_UTILS_DEQUE_H

#include <rels/include/cxx.h>
#include <rels/include/defines.h>
#include <cstddef>

namespace tpgz {
namespace containers {

template <typename T>
class deque {
private:
    struct Node {
        Node(const T& d) : data(d), next(NULL), prev(NULL) {}

        T data;
        Node* next;
        Node* prev;
    };

public:
    class iterator {
    private:
        Node* node;

    public:
        iterator(Node* pNode) : node(pNode) {}

        T& operator*() { return node->data; }

        T& operator*() const { return node->data; }

        T* operator->() { return &(node->data); }

        T* operator->() const { return &(node->data); }

        iterator& operator++() {
            node = node->next;
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator--() {
            node = node->prev;
            return *this;
        }

        iterator operator--(int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const iterator& other) const { return node == other.node; }

        bool operator!=(const iterator& other) const { return !(*this == other); }
        friend class deque;
    };

    typedef iterator const_iterator;

private:
    Node* head;
    Node* tail;
    size_t _size;

public:
    deque() : head(NULL), tail(NULL), _size(0) {}

    ~deque() { clear(); }

    void erase(iterator it) {
        Node* currentNode = it.node;

        if (currentNode == NULL) {
            return;
        }

        if (currentNode == head) {
            // The iterator points to the head node
            pop_front();
            return;
        } else if (currentNode == tail) {
            // The iterator points to the tail node
            pop_back();
            return;
        }

        // A node in the middle of the deque
        Node* prevNode = currentNode->prev;
        Node* nextNode = currentNode->next;
        prevNode->next = nextNode;
        nextNode->prev = prevNode;
        delete currentNode;
        --_size;
    }

    void push_front(const T& value) {
        Node* newNode = new Node(value);
        Node* oldHead = head;
        newNode->next = oldHead;
        head = newNode;
        if (oldHead != NULL) {
            oldHead->prev = newNode;
        }
        if (tail == NULL) {
            tail = newNode;
        }
        ++_size;
    }

    void push_back(const T& value) {
        Node* newNode = new Node(value);
        Node* oldTail = tail;
        newNode->prev = oldTail;
        tail = newNode;
        if (oldTail != NULL) {
            oldTail->next = newNode;
        }
        if (head == NULL) {
            head = newNode;
        }
        ++_size;
    }

    void pop_front() {
        Node* oldHead = head;
        if (oldHead == NULL) {
            return;
        }
        Node* newHead = oldHead->next;
        head = newHead;
        if (newHead != NULL) {
            newHead->prev = NULL;
        }
        delete oldHead;
        --_size;
        if (_size == 0) {
            tail = NULL;
        }
    }

    void pop_back() {
        Node* oldTail = tail;
        if (oldTail == NULL) {
            return;
        }
        Node* newTail = oldTail->prev;
        tail = newTail;
        if (newTail != NULL) {
            newTail->next = NULL;
        }
        delete oldTail;
        --_size;
        if (_size == 0) {
            head = NULL;
        }
    }

    T& front() { return head->data; }

    T& back() { return tail->data; }

    size_t size() const { return _size; }

    bool empty() const { return _size == 0; }

    void clear() {
        while (!empty()) {
            pop_front();
        }
    }

    void resize(size_t new_size) {
        while (_size > new_size) {
            pop_back();
        }
        while (_size < new_size) {
            push_back(T());
        }
    }

    void resize(size_t new_size, const T& value) {
        while (_size > new_size) {
            pop_back();
        }
        while (_size < new_size) {
            push_back(value);
        }
    }

    iterator begin() { return iterator(head); }

    iterator end() { return iterator(NULL); }

    const_iterator begin() const { return const_iterator(head); }

    const_iterator end() const { return const_iterator(NULL); }

    const_iterator cbegin() const { return begin(); }

    const_iterator cend() const { return end(); }
};

}
}  // namespace tpgz::containers

#endif