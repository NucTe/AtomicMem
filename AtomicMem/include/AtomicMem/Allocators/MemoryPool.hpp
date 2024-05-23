#ifndef __ATOMICMEM__ALLOCATORS__MEMORYPOOL__HPP__
#define __ATOMICMEM__ALLOCATORS__MEMORYPOOL__HPP__

#include "Allocator.hpp"
#include <stdexcept>
#include <mutex>

namespace AtomicMem {

class MemoryPool : public Allocator {
public:
    MemoryPool(size_t size);
    ~MemoryPool();

    void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) override;
    void Deallocate(void* ptr) override;

private:
    size_t m_Size;
    void* m_Memory;
    FreeBlock* m_FreeList;
    void* m_FreePtr;
    std::mutex m_Mutex;

    void* AlignForward(void* ptr, size_t alignment);
};

}

#endif // __ATOMICMEM__MEMORYPOOL__HPP__
