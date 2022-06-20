#pragma once

#include <stdlib/types.h>

void* kmalloc(usize);
void kfree(void*);

void* operator new(usize);
void operator delete(void*);
void operator delete(void*, usize);

class MemoryManager final {
public:
    static MemoryManager& the()
    {
        static MemoryManager instance;
        return instance;
    }

    void initialize(void*, usize);
    void* allocate(usize);
    void* allocate_aligned(usize, usize);
    void free(void*);
    void free_aligned(void*);

    usize allocations() const;
    usize frees() const;
    usize allocated() const;
    usize available() const;
    usize total() const;

private:
    void* m_memory_start;
    usize m_memory_size;
    usize m_allocation_count;
    usize m_allocated;
    usize m_free_count;
    usize m_free;

    bool is_kmalloc_address(const void*);

    MemoryManager() = default;

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    MemoryManager(MemoryManager&&) = delete;
    MemoryManager& operator=(MemoryManager&&) = delete;
};
