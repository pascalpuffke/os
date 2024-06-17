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

    void initialize(usize, usize);
    void* allocate(usize);
    void* allocate_aligned(usize, usize);
    void free(void*);
    void free_aligned(void*);

    [[nodiscard]] usize allocations() const;
    [[nodiscard]] usize frees() const;
    [[nodiscard]] usize allocated() const;
    [[nodiscard]] usize available() const;
    [[nodiscard]] usize total() const;

private:
    usize m_memory_start;
    usize m_memory_size;
    usize m_allocation_count;
    usize m_allocated;
    usize m_free_count;
    usize m_free;

    bool is_kmalloc_address(const void*);

    MemoryManager() = default;
};

}
