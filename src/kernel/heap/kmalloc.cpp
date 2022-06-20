// Bitmap memory allocator stolen from an old SerenityOS version.
// https://raw.githubusercontent.com/SerenityOS/serenity/1c692e87a647b26dc21825707b41b740a465a570/Kernel/Heap/kmalloc.cpp
// TODO: Replace with a proper efficient allocator.

#include <kernel/heap/kmalloc.h>
#include <kernel/util/asm.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>
#include <stdlib/array.h>

void memset(void* ptr, u8 value, u32 size)
{
    u8* p = (u8*)ptr;
    for (u32 i = 0; i < size; i++)
        p[i] = value;
}

struct [[gnu::packed]] Block {
    u32 start;
    u32 chunk;
};

// The heap is split into chunks of this size.
static constexpr usize CHUNK_SIZE = 64;
// 512 MiB ought to be enough for anybody.
// TODO: Use an algorithm to determine the best size based on the system's memory.
static constexpr usize POOL_SIZE = (512 * 1024);
// 1 bit per chunk. 1 = allocated, 0 = free.
static u8 pool[POOL_SIZE / CHUNK_SIZE / 8] __attribute__((used));

void MemoryManager::initialize(void* memory_start, usize memory_size)
{
    // To avoid writing over other data
    m_memory_start = reinterpret_cast<u8*>(memory_start) + 0x10000;
    m_memory_size = memory_size;
    m_free = POOL_SIZE;

    memset(pool, 0, POOL_SIZE / CHUNK_SIZE / 8);
    memset(m_memory_start, 0, POOL_SIZE);

    kprintf("MemoryManager initialized, %dK available @ %d byte chunks\n", POOL_SIZE / 1024, CHUNK_SIZE);
}

void* MemoryManager::allocate(usize size)
{
    Kernel::cli();
    ++m_allocation_count;

    usize real_size = size + sizeof(Block);
    kassert(m_free > real_size);

    usize chunks_needed = real_size / CHUNK_SIZE;
    if (real_size % CHUNK_SIZE)
        ++chunks_needed;

    usize chunks_here = 0;
    usize first_chunk = 0;

    for (usize i = 0; i < (POOL_SIZE / CHUNK_SIZE / 8); ++i) {
        if (pool[i] == 0xFF) {
            chunks_here = 0;
            continue;
        }

        for (usize j = 0; j < 8; ++j) {
            if (!(pool[i] & (1 << j))) {
                if (chunks_here++ == 0)
                    first_chunk = i * 8 + j;

                if (chunks_here == chunks_needed) {
                    auto* block = reinterpret_cast<Block*>((usize)m_memory_start + first_chunk * CHUNK_SIZE);
                    u8* pointer = reinterpret_cast<u8*>(block);
                    pointer += sizeof(Block);
                    block->chunk = chunks_needed;
                    block->start = first_chunk;

                    for (usize k = 0; k < first_chunk + chunks_needed; ++k) {
                        pool[first_chunk / 8] |= (1 << (first_chunk % 8));
                    }

                    m_allocated += block->chunk * CHUNK_SIZE;
                    m_free -= block->chunk * CHUNK_SIZE;

                    return pointer;
                }
            } else {
                chunks_here = 0;
            }
        }
    }

    kprintf("MemoryManager: Out of memory.\n");
    kassert(false);

    return nullptr;
}

void* MemoryManager::allocate_aligned(usize size, usize alignment)
{
    void* ptr = kmalloc(size + alignment + sizeof(void*));
    usize max_addr = reinterpret_cast<usize>(ptr) + alignment;
    void* aligned_ptr = reinterpret_cast<void*>(max_addr - (max_addr % alignment));
    ((void**)aligned_ptr)[-1] = ptr;
    return aligned_ptr;
}

void MemoryManager::free(void* ptr)
{
    if (!ptr)
        return;

    kassert(is_kmalloc_address(ptr));

    Kernel::cli();
    ++m_free_count;

    auto* block = reinterpret_cast<Block*>((usize)ptr - sizeof(Block));

    for (usize i = block->start; i < block->start + block->chunk; ++i) {
        pool[i / 8] &= ~(1 << (i % 8));
    }

    m_allocated -= block->chunk * CHUNK_SIZE;
    m_free += block->chunk * CHUNK_SIZE;
}

void MemoryManager::free_aligned(void* ptr)
{
    kfree((reinterpret_cast<void**>(ptr))[-1]);
}

bool MemoryManager::is_kmalloc_address(const void* ptr)
{
    return (usize)ptr >= (usize)m_memory_start && (usize)ptr <= (usize)m_memory_start + POOL_SIZE;
}

usize MemoryManager::allocations() const { return m_allocation_count; }

usize MemoryManager::frees() const { return m_free_count; }

usize MemoryManager::allocated() const { return m_allocated; }

usize MemoryManager::available() const { return m_free; }

usize MemoryManager::total() const { return POOL_SIZE; }

void* kmalloc(usize size) { return MemoryManager::the().allocate(size); }

void kfree(void* ptr) { MemoryManager::the().free(ptr); }

void* operator new(usize size) { return kmalloc(size); }

void* operator new[](usize size) { return kmalloc(size); }

void operator delete(void* ptr) { kfree(ptr); }

void operator delete[](void* ptr) { kfree(ptr); }

void operator delete(void* ptr, usize) { kfree(ptr); }

void operator delete[](void* ptr, usize) { kfree(ptr); }
