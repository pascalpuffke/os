#pragma once

#include <stdlib/types.h>

void* kmalloc(usize);
void kfree(void*);

void* operator new(usize);
void operator delete(void*);
void operator delete(void*, usize);

namespace Kernel {

class MemoryManager final {
public:
    static MemoryManager& get() {
        static MemoryManager instance;
        return instance;
    }

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    MemoryManager(MemoryManager&&) = delete;
    MemoryManager& operator=(MemoryManager&&) = delete;

    void initialize(u64, u64);
    void* allocate(usize);
    void* allocate_aligned(usize, usize);
    void free(void*);
    void free_aligned(void*);

    [[nodiscard]] u64 allocations() const;
    [[nodiscard]] u64 frees() const;
    [[nodiscard]] u64 allocated() const;
    [[nodiscard]] u64 available() const;
    [[nodiscard]] u64 total() const;

private:
    u64 m_memory_start;
    u64 m_memory_size;
    u64 m_allocation_count;
    u64 m_allocated;
    u64 m_free_count;
    u64 m_free;

    bool m_initialized;

    bool is_kmalloc_address(const void*);

    MemoryManager() = default;
};

}
