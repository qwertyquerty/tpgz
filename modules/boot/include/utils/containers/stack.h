#ifndef TPGZ_BOOT_UTILS_STACK_H
#define TPGZ_BOOT_UTILS_STACK_H

#include <rels/include/cxx.h>
#include <rels/include/defines.h>
#include <cstddef>

namespace tpgz {
namespace containers {

template <typename T>
class stack {
private:
    struct Node {
        Node(const T& d, Node* n) : data(d), next(n) {}

        T data;
        Node* next;
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

        bool operator==(const iterator& other) const { return node == other.node; }

        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

private:
    Node* top;

public:
    stack() : top(NULL) {}

    ~stack() { clear(); }

    void erase(iterator it) {
        Node* currentTop = top;
        Node* previousNode = NULL;

        while (currentTop != NULL) {
            if (currentTop == it.node) {
                if (previousNode == NULL) {
                    // The top node matches the iterator
                    top = currentTop->next;
                    delete currentTop;
                    currentTop = top;
                } else {
                    // A node in the middle of the stack matches the iterator
                    previousNode->next = currentTop->next;
                    delete currentTop;
                    currentTop = previousNode->next;
                }
                return;
            } else {
                previousNode = currentTop;
                currentTop = currentTop->next;
            }
        }
    }

    void push(const T& value) {
        top = new Node(value, top);
    }

    void pop() {
        Node* oldTop = top;
        if (oldTop != NULL) {
            top = oldTop->next;
            delete oldTop;
        }
    }

    bool empty() const { return top == NULL; }

    void clear() {
        while (!empty()) {
            pop();
        }
    }

    size_t size() const {
        Node* currentTop = top;
        size_t count = 0;
        while (currentTop != NULL) {
            currentTop = currentTop->next;
            ++count;
        }
        return count;
    }

    iterator begin() { return iterator(top); }

    iterator end() { return iterator(NULL); }
};

}
}  // namespace tpgz::containers

#endif