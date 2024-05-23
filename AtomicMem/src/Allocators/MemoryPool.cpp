#include "AtomicMem/Allocators/MemoryPool.hpp"
#include <cstdlib>

namespace AtomicMem {

MemoryPool::MemoryPool(size_t size)
    : m_Size(size), m_Memory(nullptr), m_FreeList(nullptr), m_FreePtr(nullptr) {
    m_Memory = std::malloc(size);
    if (!m_Memory) {
        throw std::runtime_error("Memory allocation failed");
    }
    m_FreePtr = m_Memory;
}

MemoryPool::~MemoryPool() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::free(m_Memory);
}

void* MemoryPool::AlignForward(void* ptr, size_t alignment) {
    uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
    size_t misalignment = p & (alignment - 1);
    size_t adjustment = (alignment - misalignment) & (alignment - 1);
    return reinterpret_cast<void*>(p + adjustment);
}

void* MemoryPool::Allocate(size_t size, size_t alignment) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    size_t totalSize = size + alignment - 1;
    FreeBlock* prevBlock = nullptr;
    FreeBlock* currBlock = m_FreeList;
    
    while (currBlock) {
        uintptr_t alignedPtr = reinterpret_cast<uintptr_t>(currBlock) + sizeof(FreeBlock);
        alignedPtr = (alignedPtr + alignment - 1) & ~(alignment - 1);

        size_t blockSize = 0;
        if (currBlock->next) {
            blockSize = reinterpret_cast<uintptr_t>(currBlock->next) - alignedPtr;
        } else {
            blockSize = reinterpret_cast<uintptr_t>(m_Memory) + m_Size - alignedPtr;
        }

        if (blockSize >= size) {
            if (prevBlock) {
                prevBlock->next = currBlock->next;
            } else {
                m_FreeList = currBlock->next;
            }
            return reinterpret_cast<void*>(alignedPtr);
        }

        prevBlock = currBlock;
        currBlock = currBlock->next;
    }

    void* alignedPtr = AlignForward(m_FreePtr, alignment);
    if (reinterpret_cast<uintptr_t>(alignedPtr) + size > reinterpret_cast<uintptr_t>(m_Memory) + m_Size) {
        throw std::bad_alloc();
    }

    m_FreePtr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(alignedPtr) + size);
    return alignedPtr;
}



void MemoryPool::Deallocate(void* ptr) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    FreeBlock* block = static_cast<FreeBlock*>(ptr);
    FreeBlock* prevBlock = nullptr;
    FreeBlock* currBlock = m_FreeList;
    while (currBlock && currBlock < block) {
        prevBlock = currBlock;
        currBlock = currBlock->next;
    }
    if (prevBlock && reinterpret_cast<uintptr_t>(prevBlock) + sizeof(FreeBlock) + prevBlock->size == reinterpret_cast<uintptr_t>(block)) {
        prevBlock->size += sizeof(FreeBlock) + block->size;
        block = prevBlock;
    } else {
        block->next = currBlock;
        if (prevBlock) {
            prevBlock->next = block;
        } else {
            m_FreeList = block;
        }
    }
    if (block->next && reinterpret_cast<uintptr_t>(block) + sizeof(FreeBlock) + block->size == reinterpret_cast<uintptr_t>(block->next)) {
        block->size += sizeof(FreeBlock) + block->next->size;
        block->next = block->next->next;
    }
}


}
