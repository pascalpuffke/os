#pragma once

#include <kernel/heap/kmalloc.h>
#include <libc/string.h>
#include <stdlib/assert.h>
#include <stdlib/memory/allocator.h>
#include <stdlib/move.h>
#include <stdlib/traits.h>

template <typename CharT, class Allocator = allocator<CharT>>
class BasicString final {
public:
    using value_type = CharT;
    using allocator_type = Allocator;
    using size_type = Allocator::size_type;
    using difference_type = Allocator::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = Allocator::pointer;
    using const_pointer = Allocator::const_pointer;

    static const size_type npos = size_type(-1);

    ~BasicString()
    {
        if (m_data)
            dispose();
    }

    /**
     * Default constructor.
     * Constructs empty string (zero size and unspecified capacity).
     * If no allocator is supplied, allocator is obtained from a default-constructed instance.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (1)
     */
    constexpr BasicString()
        : m_allocator(allocator_type())
    {
    }

    /**
     * Constructs the string with count copies of character ch.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (2)
     * @param count number of characters to copy
     * @param ch character to copy
     */
    constexpr BasicString(size_type count, value_type ch)
        : m_allocator(allocator_type())
    {
        assign(count, ch);
    }

    /**
     * Constructs the string with a substring [pos, pos+count) of other.
     * If count == npos, if count is not specified, or if the requested substring lasts past the end of the string, the resulting substring is [pos, other.size()).
     * @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (3)
     * @param other another string to use as source to initialize the string with
     * @param pos position of the first character to include
     * @param count size of the resulting string
     */
    constexpr BasicString(const BasicString& other, size_type pos, size_type count)
        : m_allocator(allocator_type())
    {
        assign(other, pos, count);
    }

    /**
     * Constructs the string with the first count characters of character string pointed to by s.
     * s can contain null characters.
     * The length of the string is count.
     * The behavior is undefined if [s, s + count) is not a valid range.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (4)
     * @param s pointer to an array of characters to use as source to initialize the string with
     * @param count size of the resulting string
     */
    constexpr BasicString(const_pointer s, size_type count)
        : m_allocator(allocator_type())
    {
        assign(s, count);
    }

    /**
     * Constructs the string with the contents initialized with a copy of the null-terminated character string pointed to by s.
     * The length of the string is determined by the first null character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (5)
     * @param s pointer to an array of characters to use as source to initialize the string with
     * */
    constexpr BasicString(const_pointer s)
        : m_allocator(allocator_type())
    {
        assign(s);
    }

    /**
     * Replaces the contents with count copies of character ch.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (1)
     * @param count the number of characters to copy
     * @param ch value to initialize characters of the string with
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(size_type count, value_type ch)
    {
        reserve(count);
        m_size = count;
        memset(m_data, ch, count);

        return *this;
    }

    /**
     * Replaces the contents with a copy of str.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (2)
     * @param str string to be used as source to initialize the characters with
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(const BasicString& str)
    {
        return assign(str.data(), str.size());
    }

    /**
     * Replaces the contents with a substring [pos, pos+count) of str.
     * If the requested substring lasts past the end of the string, or if count == npos, the resulting substring is [pos, str.size()).
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (3)
     * @param str string to be used as source to initialize the characters with
     * @param pos index of the first character to take
     * @param count size of the resulting string
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(const BasicString& str, size_type pos, size_type count = npos)
    {
        const auto adjusted_count = count == npos ? str.size() - pos : count;
        return assign(str.data() + pos, adjusted_count);
    }

    /**
     * Replaces the contents with those of str using move semantics.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (4)
     * @param str string to be used as source to initialize the characters with
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(BasicString&& str)
    {
        m_data = str.m_data;
        m_capacity = str.m_capacity;
        m_size = str.m_size;

        str.m_data = nullptr;
        str.m_size = 0;

        return *this;
    }

    /**
     * Replaces the contents with copies of the characters in the range [s, s+count).
     * This range can contain null characters.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (5)
     * @param s pointer to a character string to use as source to initialize the string with
     * @param count size of the resulting string
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(const_pointer s, size_type count)
    {
        reserve(count);
        m_size = count;
        memcpy(m_data, s, count);

        return *this;
    }

    /**
     * Replaces the contents with those of null-terminated character string pointed to by s.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (6)
     * @param s pointer to a character string to use as source to initialize the string with
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(const_pointer s)
    {
        return assign(s, strlen(s));
    }

    /// Element access

    /**
     * Returns a mutable reference to the character at specified location pos.
     * Bounds checking is performed.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/at
     * @param pos position of the character to return
     * @return Reference to the requested character.
     */
    constexpr reference at(size_type pos)
    {
        assert(pos < size());
        return m_data[pos];
    }

    /**
     * Returns an immutable reference to the character at specified location pos.
     * Bounds checking is performed.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/at
     * @param pos position of the character to return
     * @return Reference to the requested character.
     */
    constexpr const_reference at(size_type pos) const
    {
        assert(pos < size());
        return m_data[pos];
    }

    /**
     * Returns a mutable reference to the character at specified location pos.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/operator_at
     * @param pos position of the character to return
     * @return Mutable reference to the requested character.
     */
    constexpr reference operator[](size_type pos)
    {
        // If pos == size(), a reference to the character with value CharT() (the null character) is returned.
        if (pos == size())
            return CharT();
        return at(pos);
    }

    /**
     * Returns an immutable reference to the character at specified location pos.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/operator_at
     * @param pos position of the character to return
     * @return Immutable reference to the requested character.
     */
    constexpr const_reference operator[](size_type pos) const
    {
        // If pos == size(), a reference to the character with value CharT() (the null character) is returned.
        if (pos == size())
            return CharT();
        return at(pos);
    }

    /**
     * @brief Returns a pointer to the underlying array serving as character storage.
     * The pointer is such that the range [data(); data() + size()] is valid and the values in it correspond to the values stored in the string.
     * The returned array is null-terminated, that is, data() and c_str() perform the same function.
     * If empty() returns true, the pointer points to a single null character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/data
     * @return A pointer to the underlying character storage.
     */
    constexpr pointer data()
    {
        return m_data;
    }

    /**
     * @brief Returns a const pointer to the underlying array serving as character storage.
     * The pointer is such that the range [data(); data() + size()] is valid and the values in it correspond to the values stored in the string.
     * The returned array is null-terminated, that is, data() and c_str() perform the same function.
     * If empty() returns true, the pointer points to a single null character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/data
     * @return A const pointer to the underlying character storage.
     */
    constexpr const_pointer data() const
    {
        return m_data;
    }

    /**
     * Returns a pointer to a null-terminated character array with data equivalent to those stored in the string.
     * The pointer is such that the range [c_str(); c_str() + size()] is valid and the values in it correspond to the values stored in the string with an additional null character after the last position.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/c_str
     * @return Pointer to the underlying character storage.
     */
    constexpr const_pointer c_str() const
    {
        return m_data;
    }

    /**
     * Returns a mutable reference to the first character in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/front
     * @return mutable reference to the first character, equivalent to operator[](0).
     */
    constexpr reference front()
    {
        assert(!empty());
        return m_data[0];
    }

    /**
     * Returns an immutable reference to the first character in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/front
     * @return immutable reference to the first character, equivalent to operator[](0).
     */
    constexpr const_reference front() const
    {
        assert(!empty());
        return m_data[0];
    }

    /**
     * Returns a mutable reference to the last character in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/back
     * @return mutable reference to the first character, equivalent to operator[](size() - 1).
     */
    constexpr reference back()
    {
        assert(!empty());
        return m_data[size() - 1];
    }

    /**
     * Returns an immutable reference to the last character in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/back
     * @return immutable reference to the first character, equivalent to operator[](size() - 1).
     */
    constexpr const_reference back() const
    {
        assert(!empty());
        return m_data[size() - 1];
    }

    /// Capacity functions

    /**
     * Checks if the string has no characters, i.e. whether begin() == end().
     * @see https://en.cppreference.com/w/cpp/string/basic_string/empty
     * @return true if the string is empty, false otherwise
     */
    [[nodiscard]] constexpr bool empty() const
    {
        return size() == 0;
    }

    /**
     * Informs a BasicString object of a planned change in size, so that it can manage the storage allocation appropriately.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/reserve
     * @param capacity new capacity of the string
     */
    constexpr void reserve(size_type new_cap)
    {
        // If new_cap is greater than the current capacity(), new storage is allocated, and capacity() is made equal or greater than new_cap.
        if (new_cap > capacity())
            grow(new_cap);
        // If new_cap is less than or equal to the current capacity(), there is no effect.
    }

    /**
     * Returns the number of CharT elements in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/size
     * @return The number of CharT elements in the string.
     */
    constexpr size_type size() const
    {
        return m_size;
    }

    /**
     * Returns the number of CharT elements in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/size
     * @return The number of CharT elements in the string.
     */
    constexpr size_type length() const
    {
        return m_size;
    }

    /**
     * Requests the removal of unused capacity.
     * It is a non-binding request to reduce capacity() to size(). It depends on the implementation if the request is fulfilled.
     * If (and only if) reallocation takes place, all pointers, references, and iterators are invalidated.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/shrink_to_fit
     */
    constexpr void shrink_to_fit()
    {
        if (capacity() > size())
            grow(size());
    }

    /**
     * Returns the number of characters that the string has currently allocated space for.
     * @return Capacity of the currently allocated storage, i.e. the storage available for storing elements.
     */
    constexpr size_type capacity() const { return m_capacity; }

    /// Operations

    /**
     * Removes all characters from the string.
     * All pointers, references, and iterators are invalidated.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/clear
     */
    constexpr void clear() { m_size = 0; }

    /**
     * Inserts a string, obtained by str.substr(index_str, count) at the position index
     * @see https://en.cppreference.com/w/cpp/string/basic_string/insert
     * @param index position at which the content will be inserted
     * @param str string to insert
     * @param index_str position of the first character in the string str to insert
     * @param count number of characters to insert
     * @return *this
     */
    constexpr BasicString& insert(size_type index, const BasicString& str, size_type index_str, size_type count = npos)
    {
        assert(index <= size());
        assert(index_str <= str.length());

        auto sub_str = str.substr(index_str, count);

        return *this;
    }

    /**
     * Removes specified characters from the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/erase
     * @note This function allocates a new string
     * @param index first character to remove
     * @param count number of characters to remove
     * @return *this
     */
    constexpr BasicString& erase(size_type index = 0, size_type count = npos)
    {
        assert(index <= size());
        if (count == npos) {
            m_size = index;
            auto new_str = BasicString(m_data, index);
            *this = new_str;
            return *this;
        }
        stub();
        return *this;
    }

    /**
     * Appends the given character ch to the end of the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/push_back
     * @param ch the character to append
     */
    constexpr void push_back(value_type ch)
    {
        const auto new_size = size() + 1;
        if (new_size > capacity())
            reserve(new_size);
        m_data[m_size++] = ch;
    }

    /**
     * Removes the last character from the string.
     * Equivalent to erase(end()-1).
     * @see https://en.cppreference.com/w/cpp/string/basic_string/pop_back
     */
    constexpr void pop_back()
    {
        assert(size() > 0);
        erase(size() - 1);
    }

    /**
     * Returns a substring [pos, pos+count).
     * If the requested substring extends past the end of the string, i.e. the count is greater than size() - pos (e.g. if count == npos), the returned substring is [pos, size()).
     * @see https://en.cppreference.com/w/cpp/string/basic_string/substr
     * @param pos position of the first character to include
     * @param count length of the substring
     * @return String containing the substring [pos, pos+count) or [pos, size()).
     */
    constexpr BasicString substr(size_type pos = 0, size_type count = npos) const
    {
        assert(pos <= size());
        const auto adjusted_count = count == npos ? size() - pos : count;
        return BasicString(m_data + pos, adjusted_count);
    }

    /**
     * Appends count copies of character ch.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/append (1)
     * @param count number of characters to append
     * @param ch character value to append
     * @return reference to this string after appending
     */
    constexpr BasicString& append(size_type count, value_type ch)
    {
        if (count == 0)
            return *this;

        for (size_type i = 0; i < count; ++i)
            push_back(ch);

        return *this;
    }

    /**
     * Appends another string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/append (2)
     * @param str string to append
     * @return reference to this string after appending
     */
    constexpr BasicString& append(const BasicString& str)
    {
        return append(str.data(), str.size());
    }

    /**
     * Appends a substring [pos, pos+count) of str.
     * If the requested substring lasts past the end of the string, or if count == npos, the appended substring is [pos, size()).
     * @see https://en.cppreference.com/w/cpp/string/basic_string/append (3)
     * @param str string to append
     * @param pos the index of the first character to append
     * @param count number of characters to append
     * @return reference to this string after appending
     */
    constexpr BasicString& append(const BasicString& str, size_type pos, size_type count = npos)
    {
        assert(pos <= str.size());
        assert(count == npos || pos + count <= str.size());
        auto adjusted_count = count == npos ? str.size() - pos : count;

        for (auto i = pos; i < pos + adjusted_count; ++i)
            push_back(str.at(i));

        return *this;
    }

    /**
     * Appends characters in the range [s, s + count).
     * This range can contain null characters.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/append (4)
     * @param s pointer to the character string to append
     * @param count number of characters to append
     * @return reference to this string after appending
     */
    constexpr BasicString& append(const_pointer s, size_type count)
    {
        for (size_type i = 0; i < count; ++i)
            push_back(s[i]);

        return *this;
    }

    /**
     * Appends the null-terminated character string pointed to by s.
     * The length of the string is determined by the first null character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/append (5)
     * @param s pointer to the character string to append
     * @return reference to this string after appending
     */
    constexpr BasicString& append(const_pointer s)
    {
        return append(s, strlen(s));
    }

    /**
     * Exchanges the contents of the string with those of other.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/swap
     * @param other string to exchange the contents with
     */
    constexpr void swap(BasicString& other) noexcept
    {
        if (this == &other)
            return;

        auto temp = other;
        other = *this;
        *this = temp;
    }

private:
    constexpr void grow(size_type new_capacity, size_type grow_factor = 2)
    {
        size_type optimal_cap = capacity();
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
            memcpy(new_buffer, data(), length() + 1);

            // Get rid of the old buffer.
            dispose();
        }

        // Set the new buffer.
        m_data = new_buffer;
        m_capacity = optimal_cap;
    }

    constexpr void dispose()
    {
        m_allocator.deallocate(data(), capacity());
    }

    pointer m_data { nullptr };
    size_type m_size { 0 };
    size_type m_capacity { 0 };
    allocator_type m_allocator;
};

using String = BasicString<char>;
