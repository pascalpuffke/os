// Bitmap memory allocator stolen from an old SerenityOS version.
// https://raw.githubusercontent.com/SerenityOS/serenity/1c692e87a647b26dc21825707b41b740a465a570/Kernel/Heap/kmalloc.cpp
// TODO: Replace with a proper efficient allocator.

#include <kernel/heap/kmalloc.h>
#include <kernel/util/asm.h>
#include <kernel/util/interrupt_scope.h>
#include <kernel/util/kassert.h>
#include <kernel/util/kprintf.h>

#include <libc/string.h>

struct [[gnu::packed]] Block {
    u32 start;
    u32 chunk;
};

// Zero-initialize all new blocks.
static constexpr bool ZERO_MEMORY = true;

// The heap is split into chunks of this size.
static constexpr usize CHUNK_SIZE = 64;
static constexpr usize POOL_SIZE = (4 * MiB);
static constexpr usize BITMAP_SIZE = (POOL_SIZE / CHUNK_SIZE / 8);
// 1 bit per chunk. 1 = allocated, 0 = free.
static u8 bitmap[BITMAP_SIZE];

void MemoryManager::initialize(u64 memory_start, u64 memory_size)
{
    kassert_msg(memory_size > POOL_SIZE, "Not enough memory for the heap");

    m_memory_start = memory_start;
    m_memory_size = memory_size;
    m_free = POOL_SIZE;

    memset(bitmap, 0, BITMAP_SIZE);
    memset(reinterpret_cast<void*>(m_memory_start), 0, POOL_SIZE);

    // TODO fix this stupid kprintf bug
    // kprintf("Heap initialized @ 0x%X, %dK configured @ %d byte chunks\n", memory_start, POOL_SIZE / KiB, CHUNK_SIZE);
    // ... prints: Heap initialized @ 0x200000, 0K configured @ 64 byte chunks
    kprintf("Heap initialized @ 0x%X, ", memory_start);
    kprintf("%dK configured @ %d byte chunks\n", POOL_SIZE / KiB, CHUNK_SIZE);
    // ... prints: Heap initialized @ 0x200000, 4096K configured @ 64 byte chunks
}

void* MemoryManager::allocate(usize size)
{
    Kernel::InterruptScope scope;
    m_allocation_count++;

    auto real_size = size + sizeof(Block);
    kassert_msg(m_free > real_size, "Ran out of memory. Oops!");

    auto chunks_needed = real_size / CHUNK_SIZE;
    if (real_size % CHUNK_SIZE)
        chunks_needed++;

    auto is_chunk_free = [](usize chunk, u8 bit) {
        return (bitmap[chunk] & (1 << bit)) == 0;
    };

    usize chunks_here = 0;
    usize first_chunk = 0;
    for (usize chunk = 0; chunk < BITMAP_SIZE; chunk++) {
        if (bitmap[chunk] == 0xFF) {
            chunks_here = 0;
            continue;
        }

        for (u8 bit = 0; bit < 8; bit++) {
            if (!is_chunk_free(chunk, bit)) {
                chunks_here = 0;
                continue;
            }

            if (chunks_here == 0)
                first_chunk = chunk * 8 + bit;
            chunks_here++;

            if (chunks_here == chunks_needed) {
                auto* block = reinterpret_cast<Block*>((u32)m_memory_start + (first_chunk * CHUNK_SIZE));
                auto* pointer = reinterpret_cast<u8*>(block);
                pointer += sizeof(Block);
                block->chunk = chunks_needed;
                block->start = first_chunk;

                for (auto current_chunk = first_chunk; current_chunk < (first_chunk + chunks_needed); current_chunk++) {
                    bitmap[current_chunk / 8] |= (1 << (current_chunk % 8));
                }

                m_allocated += block->chunk * CHUNK_SIZE;
                m_free -= block->chunk * CHUNK_SIZE;

                if constexpr (ZERO_MEMORY)
                    memset(pointer, 0, real_size);

                return pointer;
            }
        }
    }

    kassert_msg(false, "MemoryManager: Out of memory.");

    return nullptr;
}

void* MemoryManager::allocate_aligned(usize size, usize alignment)
{
    auto* ptr = kmalloc(size + alignment + sizeof(void*));
    auto max_addr = reinterpret_cast<usize>(ptr) + alignment;
    auto* aligned_ptr = reinterpret_cast<void*>(max_addr - (max_addr % alignment));
    reinterpret_cast<void**>(aligned_ptr)[-1] = ptr;
    return aligned_ptr;
}

void MemoryManager::free(void* ptr)
{
    if (!ptr)
        return;

    kassert(is_kmalloc_address(ptr));

    Kernel::InterruptScope scope;
    m_free_count++;

    auto* block = reinterpret_cast<Block*>((usize)ptr - sizeof(Block));

    for (auto i = block->start; i < block->start + block->chunk; i++) {
        bitmap[i / 8] &= ~(1 << (i % 8));
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

void* kmalloc(usize size) { return MemoryManager::get().allocate(size); }
void kfree(void* ptr) { MemoryManager::get().free(ptr); }

void* operator new(usize size) { return kmalloc(size); }
void* operator new[](usize size) { return kmalloc(size); }
void operator delete(void* ptr) { kfree(ptr); }
void operator delete[](void* ptr) { kfree(ptr); }
void operator delete(void* ptr, usize) { kfree(ptr); }
void operator delete[](void* ptr, usize) { kfree(ptr); }
