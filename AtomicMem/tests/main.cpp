#include <iostream>
#include "AtomicMem/AtomicMem.hpp"

int main() {
    AtomicMem::MemoryPool memoryPool(1024);
    AtomicMem::LinkedList<int> list(memoryPool);

    list.insert(1);
    list.insert(2);
    list.insert(3);

    list.print();

    std::cout << "Press Enter to exit.";
    std::cin.ignore();
    return 0;
}

