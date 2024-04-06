#pragma once

#include <stdlib/initializer_list.h>
#include <stdlib/memory/allocator.h>
#include <stdlib/move.h>

/**
 * Vector is a dynamically-sized array.
 * @tparam T The type of the elements.
 * @tparam Allocator An allocator that is used to acquire/release memory and to construct/destroy the elements in that memory.
 */
template <typename T, typename Allocator = allocator<T>>
class Vector final {
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = typename Allocator::size_type;
    using difference_type = typename Allocator::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename Allocator::pointer;
    using const_pointer = typename Allocator::const_pointer;

    constexpr Vector()
        : m_allocator(allocator_type()) {
    }

    constexpr explicit Vector(size_type count, const_reference value) { assign(count, value); }

    constexpr explicit Vector(size_type count) { assign(count, value_type()); }

    constexpr Vector(InitializerList<value_type> il) { assign(il); }

    constexpr Vector(const Vector& other) = default;
    constexpr Vector(Vector&& other) noexcept = default;

    constexpr void assign(size_type count, const_reference value) {
        clear();
        reserve(count);
        for (size_type i = 0; i < count; ++i)
            push_back(value);
    }

    constexpr void assign(InitializerList<T> ilist) {
        clear();
        reserve(ilist.size());
        for (auto& value : ilist) {
            push_back(value);
        }
    }

    constexpr pointer data() { return m_data; }
    constexpr const_pointer data() const { return m_data; }

    constexpr size_type size() const { return m_size; }
    constexpr size_type capacity() const { return m_capacity; }

    constexpr void clear() {
        dispose();
        m_size = 0;
        // We need to leave the capacity as is, but invalidate the data pointer.
        // As I have no idea how to achieve that, we reallocate with the same capacity.
        m_data = m_allocator.allocate(capacity());
    }

    constexpr void reserve(size_type new_cap) { grow(new_cap); }

    constexpr void push_back(const_reference value) {
        const auto new_size = size() + 1;
        reserve(new_size);
        m_data[m_size++] = value;
    }

    constexpr void push_back(value_type&& value) {
        emplace_back(move(value));
    }

    template <typename... Args>
    constexpr reference emplace_back(Args&&... args) {
        const auto new_size = size() + 1;
        reserve(new_size);
        return m_data[m_size++] = value_type(forward<Args>(args)...);
    }

    constexpr const_reference operator[](size_type index) {
        return m_data[index];
    }

private:
    constexpr void grow(size_type new_capacity, size_type grow_factor = 2) {
        size_type optimal_cap = capacity(); // Capacity in value_type elements. NOT bytes!
        if (new_capacity <= optimal_cap)
            return;

        // So we don't get stuck in the while loop
        if (optimal_cap == 0)
            optimal_cap = 1;

        // Double the capacity until it is large enough.
        while (new_capacity > optimal_cap)
            optimal_cap *= grow_factor;

        // Allocate a new buffer and copy the old data into it.
        auto new_buffer = m_allocator.allocate(optimal_cap);
        if (m_data) {
            memcpy(new_buffer, data(), data_size());

            // Get rid of the old buffer.
            dispose();
        }

        // Set the new buffer.
        m_data = new_buffer;
        m_capacity = optimal_cap;
    }

    constexpr void dispose() {
        m_allocator.deallocate(data(), capacity());
        m_data = nullptr;
    }

    constexpr size_type data_size() const { return m_size * sizeof(value_type); }

    pointer m_data { nullptr };
    size_type m_size { 0 };
    size_type m_capacity { 0 };
    allocator_type m_allocator;
};
