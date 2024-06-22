#pragma once

#include <stdlib/constexpr_util.h>

/**
 * @brief An unowned, constant sequence of characters.
 * Intended as a smart replacement for C-style ´const char*´.
 */
template <typename CharT = char>
class BasicStringView final {
public:
    using value_type = CharT;
    using size_type = usize;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    static constexpr inline size_type npos = size_type(-1);

    constexpr BasicStringView() = default;

    constexpr BasicStringView(const BasicStringView&) = default;

    /**
     * @brief Constructs a StringView from a pointer to character data and a length.
     *
     * @param data pointer to the beginning of the character data
     * @param length length of the character data
     */
    constexpr BasicStringView(const_pointer data, size_type length)
        : m_data(data)
        , m_length(length) { }

    /**
     * @brief Constructs a StringView from a null-terminated character array.
     *
     * @param data pointer to an array of characters
     */
    constexpr BasicStringView(const_pointer data)
        : BasicStringView(data, cmptime::strlen(data)) { }

    constexpr BasicStringView& operator=(const BasicStringView& view) {
        m_data = view.data();
        m_length = view.size();
        return *this;
    }

    /**
     * Returns the number of characters in the view.
     * @return number of characters in the view.
     */
    [[nodiscard]] constexpr usize size() const {
        return m_length;
    }

    /**
     * Returns the number of characters in the view.
     * @return number of characters in the view.
     */
    [[nodiscard]] constexpr usize length() const {
        return m_length;
    }

    /**
     * Checks if the view has no characters.
     * @return true if the view is empty, false otherwise
     */
    [[nodiscard]] constexpr bool empty() const {
        return size() == 0;
    }

    /**
     * @brief The underlying array serving as character storage.
     * The pointer is such that the range [data(); data() + size()] is valid and the values in it correspond to the values stored in the view.
     * The returned array is null-terminated.
     * If empty() returns true, the pointer points to a single null character.
     * @return the underlying character storage
     */
    [[nodiscard]] constexpr const_pointer data() const {
        return m_data;
    }

    /**
     * Returns the first character in the view.
     * @return the first character, equivalent to operator[](0).
     */
    [[nodiscard]] constexpr value_type front() const {
        CONSTEXPR_AWARE_ASSERT(!empty());
        return m_data[0];
    }

    /**
     * Returns the last character in the view.
     * @return the last character, equivalent to operator[](size() - 1).
     */
    [[nodiscard]] constexpr value_type back() const {
        CONSTEXPR_AWARE_ASSERT(!empty());
        return m_data[size() - 1];
    }

    /**
     * @brief Exchanges the view with that of other.
     * @param other view to swap with
     */
    constexpr void swap(BasicStringView& other) {
        if (*this == other)
            return;

        auto temp = other;
        other = *this;
        *this = temp;
    }

    /**
     * @brief Moves the start of the view forward by n characters.
     * @param n number of characters to remove from the start of the view
     */
    constexpr void remove_prefix(size_type n) {
        CONSTEXPR_AWARE_ASSERT(n <= size());
        m_data += n;
        m_length -= n;
    }

    /**
     * @brief Moves the end of the view back by n characters.
     *        This does not change the underlying data at all, but only shrinks the length of the view.
     * @param n number of characters to remove from the end of the view
     */
    constexpr void remove_suffix(size_type n) {
        CONSTEXPR_AWARE_ASSERT(n <= size());

        m_length -= n;
    }

    /**
     * Returns a new subview [pos, pos+count).
     * If the requested subview extends past the end of the view, i.e. the count is greater than size() - pos (e.g. if count == npos), the returned subview is [pos, size()).
     * @param pos position of the first character to include
     * @param count length of the new subview
     * @return new view containing the subview [pos, pos+count) or [pos, size()).
     */
    [[nodiscard]] constexpr BasicStringView substr(size_type pos = 0, size_type count = npos) const {
        CONSTEXPR_AWARE_ASSERT(pos <= size());
        const auto adjusted_count = count == npos ? size() - pos : count;
        return BasicStringView {
            data() + pos,
            adjusted_count
        };
    }

    /**
     * @brief Copies the substring [pos, pos + rcount) to the character array pointed to by dest, where rcount is the smaller of count and size() - pos.
     * @param dest pointer to the destination character string
     * @param count requested substring length
     * @param pos position of the first character
     * @return number of characters copied
     */
    constexpr size_type copy(pointer dest, size_type count, size_type pos = 0) const {
        CONSTEXPR_AWARE_ASSERT(pos <= size());

        auto actual_count = (pos + count <= size()) ? count : (size() - pos);
        cmptime::memcpy(dest, data() + pos, actual_count * sizeof(value_type));

        return actual_count;
    }

    /**
     * Returns the character at specified location pos. Bounds checking is performed.
     * @param pos position of the character to return
     * @return the requested character
     */
    [[nodiscard]] constexpr value_type at(size_type pos) const {
        CONSTEXPR_AWARE_ASSERT(size() > pos);
        return m_data[pos];
    }

    /**
     * Returns the character at specified location pos. Bounds checking is performed.
     * @param pos position of the character to return
     * @return the requested character
     */
    [[nodiscard]] constexpr value_type operator[](size_type pos) const {
        return at(pos);
    }

    /**
     * Checks if the view begins with the given character.
     * @param s a single character
     * @return true if the view begins with the provided character, false otherwise
     */
    [[nodiscard]] constexpr bool starts_with(value_type c) const {
        return !empty() && at(0) == c;
    }

    /**
     * Checks if the view begins with the given prefix.
     * @param s a null-terminated character string
     * @return true if the view begins with the provided prefix, false otherwise
     */
    [[nodiscard]] constexpr bool starts_with(const_pointer s) const {
        const auto l = cmptime::strlen(s);
        if (size() < l)
            return false;
        return cmptime::memcmp(data(), s, l) == 0;
    }

    /**
     * Checks if the view ends with the given character.
     * @param s a single character
     * @return true if the view ends with the provided character, false otherwise
     */
    [[nodiscard]] constexpr bool ends_with(value_type c) const {
        return !empty() && at(size() - 1) == c;
    }

    /**
     * Checks if the view ends with the given prefix.
     * @param s a null-terminated character string
     * @return true if the view ends with the provided prefix, false otherwise
     */
    [[nodiscard]] constexpr bool ends_with(const_pointer s) const {
        const auto l = cmptime::strlen(s);
        if (size() < l)
            return false;
        return cmptime::memcmp(data() + size() - l, s, l) == 0;
    }

    /**
     * @brief Finds the first occurrence of a character sequence in the view.
     *
     * @param s character sequence to search for
     * @param pos position to start the search from
     * @param count length of the character sequence
     * @return position of the first occurrence of the character sequence, or npos if not found
     */
    [[nodiscard]] constexpr size_type find(const_pointer s, size_type pos, size_type count) const {
        if (pos > size() - count)
            return npos;

        for (size_type i = pos; i < size(); i++) {
            if (at(i) == s[0]) {
                if (cmptime::memcmp(data() + i, s, count) == 0)
                    return i;
            }
        }

        return npos;
    }

    /**
     * @brief Finds the first occurrence of a character sequence in the view.
     *
     * @param s character sequence to search for
     * @param pos position to start the search from
     * @return position of the first occurrence of the character sequence, or npos if not found
     */
    [[nodiscard]] constexpr size_type find(const_pointer s, size_type pos = 0) const {
        return find(s, pos, cmptime::strlen(s));
    }

    /**
     * @brief Finds the first occurrence of a character in the view.
     *
     * @param c character to search for
     * @param pos position to start the search from
     * @return position of the first occurrence of the character, or npos if not found
     */
    [[nodiscard]] constexpr size_type find(value_type c, size_type pos = 0) const {
        return find(&c, pos, 1);
    }

    /**
     * @brief Finds the first occurrence of another view in this view.
     *
     * @param v view to search for
     * @param pos position to start the search from
     * @return position of the first occurrence of the view, or npos if not found
     */
    [[nodiscard]] constexpr size_type find(BasicStringView v, size_type pos = 0) const {
        return find(v.data(), pos, v.size());
    }

    /**
     * @brief Checks if the string view contains the given substring
     * equivalent to return find(x) != npos;
     * @param other another view
     * @return true if the string view contains the provided substring, false otherwise
     */
    [[nodiscard]] constexpr bool contains(BasicStringView other) const {
        return find(other) != npos;
    }

    /**
     * @brief Checks if the string view contains a given single character
     * equivalent to return find(x) != npos;
     * @param other a character
     * @return true if the string view contains the provided character, false otherwise
     */
    [[nodiscard]] constexpr bool contains(value_type other) const {
        return find(other) != npos;
    }

    /**
     * @brief Checks if the string view contains the given null-terminated character string
     * equivalent to return find(x) != npos;
     * @param other another view
     * @return true if the string view contains the provided substring, false otherwise
     */
    [[nodiscard]] constexpr bool contains(const_pointer other) const {
        return find(other) != npos;
    }

    [[nodiscard]] constexpr bool operator==(const BasicStringView& other) const {
        return size() == other.size() && cmptime::memcmp(data(), other.data(), size()) == 0;
    }

    [[nodiscard]] constexpr bool operator==(const_pointer s) const {
        return size() == cmptime::strlen(s) && cmptime::memcmp(data(), s, size()) == 0;
    }

    [[nodiscard]] constexpr bool operator==(value_type c) const {
        return size() == 1 && front() == c;
    }

private:
    const_pointer m_data { nullptr };
    size_type m_length { 0 };
};

using StringView = BasicStringView<char>;
