#pragma once

#include <kernel/util/kprintf.h>
#include <stdlib/assert.h>
#include <stdlib/types.h>

class ReferenceCountedBase {
public:
    ReferenceCountedBase(const ReferenceCountedBase&) = delete;
    ReferenceCountedBase& operator=(const ReferenceCountedBase&) = delete;

    ReferenceCountedBase(ReferenceCountedBase&&) = delete;
    ReferenceCountedBase& operator=(ReferenceCountedBase&&) = delete;

    ALWAYS_INLINE void ref() {
        ASSERT(m_ref_count > 0);
        m_ref_count++;
        kprintf("ReferenceCounted::ref(): %p: %d\n", this, m_ref_count);
    }

    [[nodiscard]] usize ref_count() const { return m_ref_count; }

protected:
    ReferenceCountedBase() = default;
    ~ReferenceCountedBase() {
        ASSERT(m_ref_count == 0);
    }

    ALWAYS_INLINE usize deref_base() const {
        ASSERT(m_ref_count > 0);
        return --m_ref_count;
    }

    usize mutable m_ref_count { 1 };
};

/**
 * Abstract base class for reference counted resources.
 * @tparam T The type of the resource.
 */
template <typename T>
class ReferenceCounted : public ReferenceCountedBase {
public:
    bool unref() {
        ASSERT(m_ref_count > 0);
        const auto references = deref_base();
        kprintf("ReferenceCounted::unref(): %p: %d\n", this, m_ref_count);
        if (references == 0) {
            kprintf("ReferenceCounted::unref(): %p: deleting\n", this);
            delete static_cast<const T*>(this);
            return true;
        }
        return false;
    }
};
