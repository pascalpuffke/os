#pragma once

#include <stdlib/types.h>

namespace Kernel::Interrupts {

class PIT final {
public:
    PIT();

    void enable();
    void disable();

private:
};

}
