#ifndef __ATOMICMEM__DATASTRUCTS__LINKEDLIST__HPP__
#define __ATOMICMEM__DATASTRUCTS__LINKEDLIST__HPP__

#include "AtomicMem/Allocators/MemoryPool.hpp"

namespace AtomicMem {

    template<typename T>
    class LinkedList {
    private:
        struct Node {
            T data;
            Node* next;
        };

        MemoryPool& memoryPool;
        Node* head;

    public:
        LinkedList(MemoryPool& memPool) : memoryPool(memPool), head(nullptr) {}

        void insert(const T& value) {
            Node* newNode = static_cast<Node*>(memoryPool.Allocate(sizeof(Node)));
            if (newNode) {
                newNode->data = value;
                newNode->next = head;
                head = newNode;
            } else {
                throw std::bad_alloc();
            }
        }

        void remove(const T& value) {
            Node* current = head;
            Node* prev = nullptr;
            while (current) {
                if (current->data == value) {
                    if (prev) {
                        prev->next = current->next;
                    } else {
                        head = current->next;
                    }
                    memoryPool.Deallocate(current);
                    break;
                }
                prev = current;
                current = current->next;
            }
        }

        void print() {
            Node* current = head;
            while (current != nullptr) {
                std::cout << current->data << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }



        void search(const T& value) const {
            Node* current = head;
            while (current) {
                if (current->data == value) {
                    std::cout << "Found: " << value << std::endl;
                    return;
                }
                current = current->next;
            }
            std::cout << "Not Found: " << value << std::endl;
        }

        void insertEnd(const T& value) {
            Node* newNode = static_cast<Node*>(memoryPool.Allocate(sizeof(Node)));
            newNode->data = value;
            newNode->next = nullptr;
            if (!head) {
                head = newNode;
                return;
            }
            Node* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }

        void insertAtPosition(const T& value, size_t position) {
            if (position == 0) {
                insert(value);
                return;
            }
            Node* newNode = static_cast<Node*>(memoryPool.Allocate(sizeof(Node)));
            newNode->data = value;
            Node* current = head;
            for (size_t i = 1; i < position && current; ++i) {
                current = current->next;
            }
            if (!current) {
                throw std::out_of_range("Position out of range");
            }
            newNode->next = current->next;
            current->next = newNode;
        }

        void reverse() {
            Node* prev = nullptr;
            Node* current = head;
            Node* next = nullptr;
            while (current) {
                next = current->next;
                current->next = prev;
                prev = current;
                current = next;
            }
            head = prev;
        }

        size_t size() const {
            size_t count = 0;
            Node* current = head;
            while (current) {
                ++count;
                current = current->next;
            }
            return count;
        }

        void clear() {
            Node* current = head;
            while (current) {
                Node* next = current->next;
                memoryPool.Deallocate(current);
                current = next;
            }
            head = nullptr;
        }

        class Iterator {
        public:
            Iterator(Node* node) : m_Node(node) {}

            T& operator*() const {
                return m_Node->data;
            }

            Iterator& operator++() {
                m_Node = m_Node->next;
                return *this;
            }

            bool operator!=(const Iterator& other) const {
                return m_Node != other.m_Node;
            }

        private:
            Node* m_Node;
        };

        Iterator begin() const {
            return Iterator(head);
        }

        Iterator end() const {
            return Iterator(nullptr);
        }

        void merge(LinkedList<T>& other) {
            if (!head) {
                head = other.head;
            } else {
                Node* current = head;
                while (current->next) {
                    current = current->next;
                }
                current->next = other.head;
            }
            other.head = nullptr;
        }

    };

}

#endif