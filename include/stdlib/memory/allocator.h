#pragma once

#include <kernel/heap/kmalloc.h>
#include <stdlib/traits.h>
#include <stdlib/types.h>

/**
 * The allocator class template is the default Allocator used by all standard library containers if no user-specified allocator is provided.
 * The default allocator is stateless, that is, all instances of the given allocator are interchangeable, compare equal and can deallocate memory allocated by any other instance of the same allocator type.
 * @see https://en.cppreference.com/w/cpp/memory/allocator
 * @tparam T type of the object to allocate
 */
template <typename T>
struct allocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = usize;
    using difference_type = isize;
    using propagate_on_container_move_assignment = true_type;

    constexpr allocator() = default;
    constexpr allocator(const allocator&) = default;
    template <typename U>
    constexpr allocator(const allocator<U>&) { }

    constexpr ~allocator() = default;

    /**
     * Allocates n * sizeof(T) bytes of uninitialized storage.
     * Use of this function is ill-formed if T is an incomplete type.
     * @see https://en.cppreference.com/w/cpp/memory/allocator/allocate
     * @param n the number of objects to allocate storage for
     * @return Pointer to the first element of an array of n objects of type T whose elements have not been constructed yet.
     */
    [[nodiscard]] constexpr pointer allocate(size_type n) {
        // Use of this function is ill-formed if T is an incomplete type.
        static_assert(sizeof(value_type) != 0, "Cannot allocate an object of an incomplete type");

        // Return n * sizeof(T) bytes of uninitialized storage.
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }

    /**
     * Deallocates the storage referenced by the pointer p, which must be a pointer obtained by an earlier call to allocate().
     * The argument n must be equal to the first argument of the call to allocate() that originally produced p;
     * otherwise, the behavior is undefined.
     * @see https://en.cppreference.com/w/cpp/memory/allocator/deallocate
     * @param p pointer obtained from allocate()
     */
    constexpr void deallocate(pointer p, size_type n) { ::operator delete(p, n); }
};