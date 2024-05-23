#ifndef __ATOMICMEM__ALLOCATORS__ALLOCATOR__HPP
#define __ATOMICMEM__ALLOCATORS__ALLOCATOR__HPP

#include <cstddef>

namespace AtomicMem {

class Allocator {
public:
    virtual ~Allocator() = default;
    virtual void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
    virtual void Deallocate(void* ptr) = 0;

    struct FreeBlock {
        size_t size;
        FreeBlock* next;
    };
};

}

#endif // __ATOMICMEM__ALLOCATOR__HPP
