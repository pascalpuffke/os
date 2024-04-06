#pragma once

#include <stdlib/constexpr_util.h>
#include <stdlib/memory/allocator.h>
#include <stdlib/move.h>
#include <stdlib/traits.h>

/**
 * @brief Owned String-type with various mutating functionality and resizing
 *
 * @tparam CharT
 * @tparam Allocator
 */
template <typename CharT, class Allocator = allocator<CharT>>
class BasicString final {
public:
    using value_type = CharT;
    using allocator_type = Allocator;
    using size_type = typename Allocator::size_type;
    using difference_type = typename Allocator::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename Allocator::pointer;
    using const_pointer = typename Allocator::const_pointer;

    static const size_type npos = size_type(-1);

    constexpr ~BasicString() {
        dispose();
    }

    /**
     * Default constructor.
     * Constructs empty string (zero size and unspecified capacity).
     * If no allocator is supplied, allocator is obtained from a default-constructed instance.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (1)
     */
    constexpr BasicString()
        : m_allocator(allocator_type()) {
    }

    /**
     * Constructs the string with count copies of character ch.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (2)
     * @param count number of characters to copy
     * @param ch character to copy
     */
    constexpr BasicString(size_type count, value_type ch)
        : m_allocator(allocator_type()) {
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
        : m_allocator(allocator_type()) {
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
        : m_allocator(allocator_type()) {
        assign(s, count);
    }

    /**
     * Constructs the string with the contents initialized with a copy of the null-terminated character string pointed to by s.
     * The length of the string is determined by the first null character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (5)
     * @param s pointer to an array of characters to use as source to initialize the string with
     * */
    constexpr BasicString(const_pointer s)
        : m_allocator(allocator_type()) {
        assign(s);
    }

    /**
     * Replaces the contents with count copies of character ch.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (1)
     * @param count the number of characters to copy
     * @param ch value to initialize characters of the string with
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(size_type count, value_type ch) {
        reserve(count);
        m_size = count;
        cmptime::memset(m_data, ch, count);

        return *this;
    }

    /**
     * Replaces the contents with a copy of str.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (2)
     * @param str string to be used as source to initialize the characters with
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(const BasicString& str) { return assign(str.data(), str.size()); }

    /**
     * Replaces the contents with a substring [pos, pos+count) of str.
     * If the requested substring lasts past the end of the string, or if count == npos, the resulting substring is [pos, str.size()).
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (3)
     * @param str string to be used as source to initialize the characters with
     * @param pos index of the first character to take
     * @param count size of the resulting string
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(const BasicString& str, size_type pos, size_type count = npos) {
        const auto adjusted_count = count == npos ? str.size() - pos : count;
        return assign(str.data() + pos, adjusted_count);
    }

    /**
     * Replaces the contents with those of str using move semantics.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (4)
     * @param str string to be used as source to initialize the characters with
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(BasicString&& str) {
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
    constexpr BasicString& assign(const_pointer s, size_type count) {
        reserve(count);
        m_size = count;
        cmptime::memcpy(m_data, s, count);

        return *this;
    }

    /**
     * Replaces the contents with those of null-terminated character string pointed to by s.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/assign (6)
     * @param s pointer to a character string to use as source to initialize the string with
     * @return *this with replaced contents
     */
    constexpr BasicString& assign(const_pointer s) { return assign(s, cmptime::strlen(s)); }

    /// Element access

    /**
     * Returns a mutable reference to the character at specified location pos.
     * Bounds checking is performed.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/at
     * @param pos position of the character to return
     * @return Reference to the requested character.
     */
    constexpr reference at(size_type pos) {
        CONSTEXPR_AWARE_ASSERT(pos < size());
        return m_data[pos];
    }

    /**
     * Returns an immutable reference to the character at specified location pos.
     * Bounds checking is performed.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/at
     * @param pos position of the character to return
     * @return Reference to the requested character.
     */
    constexpr const_reference at(size_type pos) const {
        CONSTEXPR_AWARE_ASSERT(pos < size());
        return m_data[pos];
    }

    /**
     * Returns a mutable reference to the character at specified location pos.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/operator_at
     * @param pos position of the character to return
     * @return Mutable reference to the requested character.
     */
    constexpr reference operator[](size_type pos) {
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
    constexpr const_reference operator[](size_type pos) const {
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
    constexpr pointer data() { return m_data; }

    /**
     * @brief Returns a const pointer to the underlying array serving as character storage.
     * The pointer is such that the range [data(); data() + size()] is valid and the values in it correspond to the values stored in the string.
     * The returned array is null-terminated, that is, data() and c_str() perform the same function.
     * If empty() returns true, the pointer points to a single null character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/data
     * @return A const pointer to the underlying character storage.
     */
    constexpr const_pointer data() const { return m_data; }

    /**
     * Returns a pointer to a null-terminated character array with data equivalent to those stored in the string.
     * The pointer is such that the range [c_str(); c_str() + size()] is valid and the values in it correspond to the values stored in the string with an additional null character after the last position.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/c_str
     * @return Pointer to the underlying character storage.
     */
    constexpr const_pointer c_str() const { return m_data; }

    /**
     * Returns a mutable reference to the first character in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/front
     * @return mutable reference to the first character, equivalent to operator[](0).
     */
    constexpr reference front() {
        CONSTEXPR_AWARE_ASSERT(!empty());
        return m_data[0];
    }

    /**
     * Returns an immutable reference to the first character in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/front
     * @return immutable reference to the first character, equivalent to operator[](0).
     */
    constexpr const_reference front() const {
        CONSTEXPR_AWARE_ASSERT(!empty());
        return m_data[0];
    }

    /**
     * Returns a mutable reference to the last character in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/back
     * @return mutable reference to the first character, equivalent to operator[](size() - 1).
     */
    constexpr reference back() {
        CONSTEXPR_AWARE_ASSERT(!empty());
        return m_data[size() - 1];
    }

    /**
     * Returns an immutable reference to the last character in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/back
     * @return immutable reference to the first character, equivalent to operator[](size() - 1).
     */
    constexpr const_reference back() const {
        CONSTEXPR_AWARE_ASSERT(!empty());
        return m_data[size() - 1];
    }

    /// Capacity functions

    /**
     * Checks if the string has no characters, i.e. whether begin() == end().
     * @see https://en.cppreference.com/w/cpp/string/basic_string/empty
     * @return true if the string is empty, false otherwise
     */
    [[nodiscard]] constexpr bool empty() const { return size() == 0; }

    /**
     * Returns the number of CharT elements in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/size
     * @return The number of CharT elements in the string.
     */
    constexpr size_type size() const { return m_size; }

    /**
     * Returns the number of CharT elements in the string.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/size
     * @return The number of CharT elements in the string.
     */
    constexpr size_type length() const { return m_size; }

    /**
     * Returns the maximum number of elements the string is able to hold due to system or library implementation limitations.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/max_size
     * @return Maximum number of characters.
     */
    constexpr size_type max_size() const {
        // TODO: Implement numeric_limits<size_type>
        return 0x7FFFFFFF;
    }

    /**
     * Informs a BasicString object of a planned change in size, so that it can manage the storage allocation appropriately.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/reserve
     * @param capacity new capacity of the string
     */
    constexpr void reserve(size_type new_cap) { grow(new_cap, false); }

    /**
     * Returns the number of characters that the string has currently allocated space for.
     * @return Capacity of the currently allocated storage, i.e. the storage available for storing elements.
     */
    constexpr size_type capacity() const { return m_capacity; }

    /**
     * Requests the removal of unused capacity.
     * It is a non-binding request to reduce capacity() to size(). It depends on the implementation if the request is fulfilled.
     * If (and only if) reallocation takes place, all pointers, references, and iterators are invalidated.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/shrink_to_fit
     */
    constexpr void shrink_to_fit() { grow(size(), true); }

    /// Operations

    /**
     * Removes all characters from the string.
     * All pointers, references, and iterators are invalidated.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/clear
     */
    constexpr void clear() {
        // Leave the capacity and the underlying data untouched.
        for (size_type i = 0; i < capacity(); i++)
            m_data[i] = CharT {};
        m_size = 0;
    }

    /**
     * Inserts a string, obtained by str.substr(index_str, count) at the position index
     * @see https://en.cppreference.com/w/cpp/string/basic_string/insert
     * @param index position at which the content will be inserted
     * @param str string to insert
     * @param index_str position of the first character in the string str to insert
     * @param count number of characters to insert
     * @return *this
     */
    constexpr BasicString& insert(size_type index, const BasicString& str, size_type index_str, size_type count = npos) {
        CONSTEXPR_AWARE_ASSERT(index <= size());
        CONSTEXPR_AWARE_ASSERT(index_str <= str.length());

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
    constexpr BasicString& erase(size_type index = 0, size_type count = npos) {
        CONSTEXPR_AWARE_ASSERT(index <= size());
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
    constexpr void push_back(value_type ch) {
        const auto new_size = size() + 1;
        reserve(new_size);
        m_data[m_size++] = ch;

        assert(m_data[size()] == '\0');
    }

    constexpr void show_internal_data() const {
        if (size() == 0) {
            kprintln("%p: (empty)", data());
            return;
        }

        kprintf("%p: ", data());
        for (size_type i = 0; i < capacity(); i++) {
            auto ch = m_data[i] == '\0' ? '0' : m_data[i];
            if (i == capacity() - 1)
                kprintln("'%c'", ch);
            else
                kprintf("'%c', ", ch);
        }
    }

    /**
     * Removes the last character from the string.
     * Equivalent to erase(end()-1).
     * @see https://en.cppreference.com/w/cpp/string/basic_string/pop_back
     */
    constexpr void pop_back() {
        CONSTEXPR_AWARE_ASSERT(size() > 0);
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
    constexpr BasicString substr(size_type pos = 0, size_type count = npos) const {
        CONSTEXPR_AWARE_ASSERT(pos <= size());
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
    constexpr BasicString& append(size_type count, value_type ch) {
        if (count == 0)
            return *this;
        reserve(size() + count);

        for (size_type i = 0; i < count; ++i)
            push_back(ch);

        return *this;
    }

    /**
     * Appends another String.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/append (2)
     * @param str string to append
     * @return reference to this string after appending
     */
    constexpr BasicString& append(const BasicString& str) { return append(str.data(), str.size()); }

    /**
     * Appends a substring [pos, pos+count) of str.
     * If the requested substring lasts past the end of the string, or if count == npos, the appended substring is [pos, size()).
     * @see https://en.cppreference.com/w/cpp/string/basic_string/append (3)
     * @param str string to append
     * @param pos the index of the first character to append
     * @param count number of characters to append
     * @return reference to this string after appending
     */
    constexpr BasicString& append(const BasicString& str, size_type pos, size_type count = npos) {
        CONSTEXPR_AWARE_ASSERT(pos <= str.size());
        CONSTEXPR_AWARE_ASSERT(count == npos || pos + count <= str.size());
        auto adjusted_count = count == npos ? str.size() - pos : count;
        reserve(size() + adjusted_count);

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
    constexpr BasicString& append(const_pointer s, size_type count) {
        reserve(size() + count);
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
    constexpr BasicString& append(const_pointer s) { return append(s, cmptime::strlen(s)); }

    /**
     * Appends another String.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/operator+= (1)
     * @param str string to append
     * @return reference to this string after appending
     */
    constexpr BasicString& operator+=(const BasicString& str) { return append(str); }

    /**
     * Appends a single character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/operator+= (2)
     * @param ch character to append
     * @return reference to this string after appending
     */
    constexpr BasicString& operator+=(value_type ch) {
        push_back(ch);
        return *this;
    }

    /**
     * Appends the null-terminated character string pointed to by s.
     * The length of the string is determined by the first null character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/operator+= (3)
     * @param s pointer to the character string to append
     * @return reference to this string after appending
     */
    constexpr BasicString& operator+=(const_pointer s) { return append(s); }

    /**
     * Checks if the String begins with the given prefix.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/starts_with (1)
     * @param s another String
     * @return true if the String begins with the provided prefix, false otherwise
     */
    // NOTE: the C++ standard wants me to use a basic_string_view param, but I have yet to implement that.
    constexpr bool starts_with(const BasicString& str) const { return size() >= str.size() && memcmp(data(), str.data(), str.size()) == 0; }

    /**
     * Checks if the String begins with the given character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/starts_with (2)
     * @param c a single character
     * @return true if the String begins with the provided character, false otherwise
     */
    constexpr bool starts_with(value_type c) const {
        return size() > 0 && at(0) == c;
    }

    /**
     * Checks if the String begins with the given prefix.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/starts_with (3)
     * @param s a null-terminated character string
     * @return true if the String begins with the provided prefix, false otherwise
     */
    constexpr bool starts_with(const_pointer s) const {
        const auto l = cmptime::strlen(s);
        if (size() < l)
            return false;
        return memcmp(data(), s, l) == 0;
    }

    /**
     * Checks if the String ends with the given suffix.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/ends_with (1)
     * @param s another String
     * @return true if the String ends with the provided suffix, false otherwise
     */
    // NOTE: the C++ standard wants me to use a basic_string_view param, but I have yet to implement that.
    constexpr bool ends_with(const BasicString& str) const {
        if (size() < str.size())
            return false;
        return memcmp(data() + size() - str.size(), str.data(), str.size()) == 0;
    }

    /**
     * Checks if the String ends with the given character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/ends_with (2)
     * @param c a single character
     * @return true if the String ends with the provided character, false otherwise
     */
    constexpr bool ends_with(value_type c) const {
        return !empty() && at(size() - 1) == c;
    }

    /**
     * Checks if the String ends with the given suffix.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/ends_with (3)
     * @param s a null-terminated character string
     * @return true if the String ends with the provided suffix, false otherwise
     */
    constexpr bool ends_with(const_pointer s) const {
        if (size() < cmptime::strlen(s))
            return false;
        return memcmp(data() + size() - cmptime::strlen(s), s, cmptime::strlen(s)) == 0;
    }

    /**
     * Exchanges the contents of the string with those of other.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/swap
     * @param other string to exchange the contents with
     */
    constexpr void swap(BasicString& other) noexcept {
        if (this == &other)
            return;

        auto temp = other;
        other = *this;
        *this = temp;
    }

    /**
     * Checks if the String contains the given substring.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/contains (1)
     * @param str another String
     * @return true if the string contains the substring, false otherwise
     */
    constexpr bool contains(const BasicString& str) const { return find(str) != npos; }

    /**
     * Checks if the String contains the given character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/contains (2)
     * @param c A single character
     * @return true if the string contains the character, false otherwise
     */
    constexpr bool contains(value_type c) const { return find(c) != npos; }

    /**
     * Checks if the String contains the given substring.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/contains (3)
     * @param s A null-terminated character string
     * @return true if the string contains the substring, false otherwise
     */
    constexpr bool contains(const_pointer s) const { return find(s) != npos; }

    /**
     * Replaces the part of the String indicated by [pos, pos + count) with a new String.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/replace (1)
     * @param pos Start of the substring that is going to be replaced
     * @param count Length of the substring that is going to be replaced
     * @param str String to use for replacement
     * @return reference to this String after replacement
     */
    constexpr BasicString& replace(size_type pos, size_type count, const BasicString& str) { return replace(pos, count, str, 0, str.size()); }

    /**
     * Replaces the part of the String indicated by [pos, pos + count) with a new String.
     * The new string is a substring [pos2, pos2 + count2) of str, except if count2==npos or if would extend past str.size(), [pos2, str.size()) is used.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/replace (2)
     * @param from Start of the substring that is going to be replaced
     * @param count Length of the substring that is going to be replaced
     * @param str String to use for replacement
     * @param str_from Start of the substring to replace with
     * @param str_len Number of characters to replace with
     * @return reference to this String after replacement
     */
    constexpr BasicString& replace(size_type from, size_type count, const BasicString& str, size_type str_from, size_type str_len = npos) {
        CONSTEXPR_AWARE_ASSERT(from <= length());
        CONSTEXPR_AWARE_ASSERT(str_from <= str.length());
        return *this;
    }

    /**
     * Replaces the part of the String indicated by [pos, pos + count) with a new String.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/replace (4)
     * @param from Start of the substring that is going to be replaced
     * @param count Length of the substring that is going to be replaced
     * @param cstr Pointer to the character string to use for replacement
     * @param cstr_len Number of characters to replace with
     * @return reference to this String after replacement
     */
    constexpr BasicString& replace(size_type from, size_type count, const_pointer cstr, size_type cstr_len) {
        CONSTEXPR_AWARE_ASSERT(from <= length());

        const auto new_size = length() - count + cstr_len;
        reserve(new_size);

        if (from < length() - count)
            move(from + count, length() - from - count, from);

        fill(from, cstr_len, cstr);

        return *this;
    }

    constexpr BasicString& replace(size_type from, size_type count, const_pointer cstr) { return replace(from, count, cstr, cmptime::strlen(cstr)); }

    constexpr BasicString& replace(size_type from, size_type count, size_type ch_count, value_type ch) {
        CONSTEXPR_AWARE_ASSERT(from <= length());

        if (ch_count == npos)
            ch_count = length() - from;

        if (ch_count == 0)
            return *this;

        if (ch_count == count) {
            fill(from, ch_count, ch);
            return *this;
        }

        const auto new_size = length() - count + ch_count;
        reserve(new_size);

        if (from < length() - count)
            move(from + ch_count, length() - count - from, from);

        fill(from, ch_count, ch);

        return *this;
    }

    /// Search

    /**
     * Finds the first substring equal to str.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/find (1)
     * @param str String to search for
     * @param pos Position at which to start the search
     * @return Position of the first character of the found substring or npos if no such substring is found
     */
    constexpr size_type find(const BasicString& str, size_type pos = 0) const {
        return find(str.data(), pos, str.size());
    }

    /**
     * Finds the first substring equal to the range [s, s+count).
     * This range may contain null characters.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/find (2)
     * @param s Pointer to a character string to search for
     * @param pos Position at which to start the search
     * @param count Length of substring to search for
     * @return Position of the first character of the found substring or npos if no such substring is found
     */
    constexpr size_type find(const_pointer s, size_type pos, size_type count) const {
        // a substring can be found only if pos <= size() - count
        if (pos > size() - count)
            return npos;

        // search for the first character of s in the string
        for (size_type i = pos; i < size(); i++) {
            // if the first character of s is found, search for the rest of s in the string
            if (at(i) == s[0]) {
                // if the rest of s is found, return the position of the first character of s
                if (memcmp(data() + i, s, count) == 0)
                    return i;
            }
        }

        return npos;
    }

    /**
     * Finds the first substring equal to the character string pointed to by s.
     * The length of the string is determined by the first null character.
     * @see https://en.cppreference.com/w/cpp/string/basic_string/find (3)
     * @param s Pointer to a character string to search for
     * @param pos Position at which to start the search
     * @return Position of the first character of the found substring or npos if no such substring is found
     */
    constexpr size_type find(const_pointer s, size_type pos = 0) const {
        return find(s, pos, cmptime::strlen(s));
    }

    /**
     * Finds the first character ch (treated as a single-character substring)
     * @see https://en.cppreference.com/w/cpp/string/basic_string/find (4)
     * @param ch Character to search for
     * @param pos Position at which to start the search
     * @return Position of the first character of the found substring or npos if no such substring is found
     */
    constexpr size_type find(value_type ch, size_type pos = 0) const {
        return find(&ch, pos, 1);
    }

private:
    /**
     * Move a substring to a new position.
     * This function will not grow the String.
     * @param from Start of the substring to move
     * @param count Length of the substring to move
     * @param to Position to move the substring to
     */
    constexpr void move(size_type from, size_type count, size_type to) {
        CONSTEXPR_AWARE_ASSERT(to + count <= capacity());
        CONSTEXPR_AWARE_ASSERT(from + count <= size());

        if (from == to)
            return;
        if (from > to) {
            for (size_type i = from; i > to; i--)
                m_data[i] = m_data[i - count];
        } else {
            for (size_type i = from; i < to; i++)
                m_data[i] = m_data[i + count];
        }
    }

    /**
     * Fill a substring with a character.
     * This function will not grow the String.
     * @param from Start of the substring to fill
     * @param count Length of the substring to fill
     * @param ch Character to fill the substring with
     */
    constexpr void fill(size_type from, size_type count, value_type ch) {
        CONSTEXPR_AWARE_ASSERT(from <= size());
        CONSTEXPR_AWARE_ASSERT(from + count <= capacity());

        for (size_type i = from; i < from + count; i++)
            m_data[i] = ch;
    }

    /**
     * Fill a substring with a character string.
     * This function will not grow the String.
     * @param from Start of the substring to fill
     * @param count Length of the substring to fill
     * @param cstr Pointer to a character string to fill the substring with
     */
    constexpr void fill(size_type from, size_type count, const_pointer cstr) {
        CONSTEXPR_AWARE_ASSERT(from <= size());
        CONSTEXPR_AWARE_ASSERT(from + count <= capacity());

        for (size_type i = from; i < from + count; i++)
            m_data[i] = cstr[i - from];
    }

    constexpr void grow(size_type new_capacity, bool allow_shrinking = false, size_type grow_factor = 2) {
        size_type optimal_cap = capacity();
        if (new_capacity <= optimal_cap && !allow_shrinking)
            return;

        if (allow_shrinking && new_capacity < optimal_cap) {
            optimal_cap = new_capacity;
        } else {
            // So we don't get stuck in the while loop
            if (optimal_cap == 0)
                optimal_cap = 1;

            // Multiply the optimal capacity by the growth factor until it is large enough
            while (new_capacity > optimal_cap)
                optimal_cap *= grow_factor;
        }

        // Allocate a new buffer and copy the old data into it.
        auto new_buffer = m_allocator.allocate(optimal_cap);
        if (m_data) {
            cmptime::memcpy(new_buffer, data(), length() + 1);

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

    pointer m_data { nullptr };
    size_type m_size { 0 };
    size_type m_capacity { 0 };
    allocator_type m_allocator;
};

using String = BasicString<char>;
