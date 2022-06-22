#pragma once

#include <stdlib/assert.h>
#include <stdlib/memory/reference.h>
#include <stdlib/memory/unique_ptr.h>
#include <stdlib/move.h>
#include <stdlib/types.h>

template <typename T>
ALWAYS_INLINE void ref_if_nonnull(T* ptr)
{
    if (ptr)
        ptr->ref();
}

template <typename T>
ALWAYS_INLINE void unref_if_nonnull(T* ptr)
{
    if (ptr)
        ptr->unref();
}

/**
 * Smart pointer for shared ownership. Resources are automatically freed when the reference count reaches 0.
 * @tparam T The type of the resource.
 */
template <typename T>
class SharedPtr final : public ReferenceCounted<T> {
public:
    using Type = remove_extent_t<T>;

    constexpr SharedPtr() = default;

    constexpr SharedPtr(Type* ptr)
        : m_ptr(ptr)
    {
        ref_if_nonnull(m_ptr);
    }

    constexpr SharedPtr(const SharedPtr& other)
        : m_ptr(other.m_ptr)
    {
        m_ptr->ref();
    }

    constexpr SharedPtr(const UniquePtr<Type>& other)
        : m_ptr(other.release())
    {
        ref_if_nonnull(m_ptr);
    }

    template <typename... Args>
    [[nodiscard]] static constexpr SharedPtr make(Args&&... args)
    {
        return SharedPtr(new Type(forward<Args>(args)...));
    }

    constexpr void reset(Type* ptr = nullptr)
    {
        unref_if_nonnull(m_ptr);
        m_ptr = ptr;
        ref_if_nonnull(m_ptr);
    }

    constexpr void swap(SharedPtr& other)
    {
        auto tmp = m_ptr;
        m_ptr = other.m_ptr;
        other.m_ptr = tmp;
    }

    constexpr Type* get() const { return m_ptr; }
    constexpr Type* operator->() const { return get(); }
    constexpr Type& operator*() const { return *get(); }

    constexpr usize ref_count() const { return ref_count(); }

    constexpr bool unique() const { return ref_count() == 1; }

    constexpr operator bool() const { return m_ptr != nullptr; }

private:
    Type* m_ptr { nullptr };
};
