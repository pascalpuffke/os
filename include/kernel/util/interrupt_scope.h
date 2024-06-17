#pragma once

#include <kernel/util/asm.h>

namespace Kernel {

/**
 * Disables interrupts at construction, re-enabling them at destruction if they were enabled before.
 */
class InterruptScope final {
public:
    InterruptScope() {
        m_if_set = has_flag(CPUFlag::InterruptEnable);
        cli();
    }

    ~InterruptScope() {
        if (m_if_set)
            sti();
    }

private:
    bool m_if_set;
};

}
