#pragma once

#include <stdlib/types.h>

void* kmalloc(usize);
void kfree(void*);

void* operator new(usize);
void operator delete(void*);
void operator delete(void*, usize);

class MemoryManager final {
public:
    static MemoryManager& get()
    {
        static MemoryManager instance;
        return instance;
    }

    void initialize(u64, u64);
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
    u64 m_memory_start;
    u64 m_memory_size;
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
