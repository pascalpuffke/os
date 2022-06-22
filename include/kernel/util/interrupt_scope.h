#pragma once

#include <kernel/util/asm.h>

namespace Kernel {

/**
 * Disables interrupts at construction, re-enabling them at destruction if they were enabled before.
 */
class InterruptScope final {
public:
    InterruptScope()
    {
        m_were_interrupts_enabled = eflags() & CPU_FLAG_IF;
        cli();
    }

    ~InterruptScope()
    {
        if (m_were_interrupts_enabled)
            sti();
    }

private:
    bool m_were_interrupts_enabled;
};

}
