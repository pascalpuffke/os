#pragma once

#include <stdlib/types.h>

namespace Kernel {

enum class CPUIDRequest {
    GET_VENDOR_STRING = 0x00,
    GET_FEATURES = 0x01,
};

enum class CPUFeature {
    ACPI,
    APIC,
    CLFLUSH,
    CMOV,
    CX8,
    DE,
    DS,
    FPU,
    FXSR,
    HTT,
    IA64,
    MCA,
    MCE,
    MMX,
    MSR,
    MTRR,
    PAE,
    PAT,
    PBE,
    PGE,
    PSE,
    PSE36,
    PSN,
    SEP,
    SS,
    SSE,
    SSE2,
    TM,
    TSC,
    TSC_DEADLINE,
    VME,
    AES,
    AVX,
    CID,
    CX16,
    DCA,
    DS_CPL,
    DTES64,
    EST,
    F16C,
    FMA,
    HYPERVISOR,
    MONITOR,
    MOVBE,
    OSXSAVE,
    PCID,
    PCLMUL,
    PDCM,
    POPCNT,
    RDRAND,
    SDBG,
    SMX,
    SSE3,
    SSE4_1,
    SSE4_2,
    SSSE3,
    TM2,
    VMX,
    X2APIC,
    XSAVE,
    XTPR,
    _LAST
};

static const char* cpu_feature_to_string(CPUFeature feature)
{
    // dear god.
    return (const char*[]) {
        "acpi", "apic", "clflush", "cmov", "cx8", "de", "ds", "fpu", "fxsr", "htt", "ia64", "mca", "mce", "mmx", "msr", "mtrr", "pae", "pat", "pbe", "pge", "pse", "pse36", "psn", "sep", "ss", "sse", "sse2", "tm", "tsc", "tsc_deadline", "vme", "aes", "avx", "cid", "cx16", "dca", "ds_cpl", "dtes64", "est", "f16c", "fma", "hypervisor", "monitor", "movbe", "osxsave", "pcid", "pclmul", "pdcm", "popcnt", "rdrand", "sdbg", "smx", "sse3", "sse4_1", "sse4_2", "ssse3", "tm2", "vmx", "x2apic", "xsave", "xtpr", ""
    }[(usize)feature];
}

class CPUID final {
public:
    const char* vendor();
    bool has_feature(CPUFeature);

private:
    u32 m_eax, m_ebx, m_ecx, m_edx;
    void get(CPUIDRequest, u32 ecx = 0);
};

}
