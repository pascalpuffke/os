#include <kernel/processor/cpuid.h>

#include <kernel/util/asm.h>
#include <kernel/util/kprintf.h>

#include <kernel/video/tty.h>
#include <kernel/video/vga.h>

#include <libc/string.h>

namespace Kernel {

// CPU vendor strings
static constexpr const char* VENDOR_OLDAMD = "AMDisbetter!";
static constexpr const char* VENDOR_AMD = "AuthenticAMD";
static constexpr const char* VENDOR_INTEL = "GenuineIntel";
static constexpr const char* VENDOR_VIA = "VIA VIA VIA ";
static constexpr const char* VENDOR_OLDTRANSMETA = "TransmetaCPU";
static constexpr const char* VENDOR_TRANSMETA = "GenuineTMx86";
static constexpr const char* VENDOR_CYRIX = "CyrixInstead";
static constexpr const char* VENDOR_CENTAUR = "CentaurHauls";
static constexpr const char* VENDOR_NEXGEN = "NexGenDriven";
static constexpr const char* VENDOR_UMC = "UMC UMC UMC ";
static constexpr const char* VENDOR_SIS = "SiS SiS SiS ";
static constexpr const char* VENDOR_NSC = "Geode by NSC";
static constexpr const char* VENDOR_RISE = "RiseRiseRise";
static constexpr const char* VENDOR_VORTEX = "Vortex86 SoC";
static constexpr const char* VENDOR_OLDAO486 = "GenuineAO486";
static constexpr const char* VENDOR_AO486 = "MiSTer AO486";
static constexpr const char* VENDOR_ZHAOXIN = "  Shanghai  ";
static constexpr const char* VENDOR_HYGON = "HygonGenuine";
static constexpr const char* VENDOR_ELBRUS = "E2K MACHINE ";
// Hypervisors
static constexpr const char* VENDOR_QEMU = "TCGTCGTCGTCG";
static constexpr const char* VENDOR_KVM = " KVMKVMKVM  ";
static constexpr const char* VENDOR_VMWARE = "VMwareVMware";
static constexpr const char* VENDOR_VIRTUALBOX = "VBoxVBoxVBox";
static constexpr const char* VENDOR_XEN = "XenVMMXenVMM";
static constexpr const char* VENDOR_HYPERV = "Microsoft Hv";
static constexpr const char* VENDOR_PARALLELS = " prl hyperv ";
static constexpr const char* VENDOR_PARALLELS_ALT = " lrpepyh vr ";
static constexpr const char* VENDOR_BHYVE = "bhyve bhyve ";
static constexpr const char* VENDOR_QNX = " QNXQVMBSQG ";

void CPUID::get(CPUIDRequest request, u32 ecx)
{
    asm volatile("cpuid"
                 : "=a"(m_eax), "=b"(m_ebx), "=c"(m_ecx), "=d"(m_edx)
                 : "a"(request), "c"(ecx));
}

const char* CPUID::vendor()
{
    get(CPUIDRequest::GET_VENDOR_STRING);

    char vendor[13];
    for (auto i = 0; i < 4; i++)
        vendor[i] = static_cast<char>(m_ebx >> (i * 8));
    for (auto i = 0; i < 4; i++)
        vendor[i + 4] = static_cast<char>(m_edx >> (i * 8));
    for (auto i = 0; i < 4; i++)
        vendor[i + 8] = static_cast<char>(m_ecx >> (i * 8));
    vendor[12] = '\0';

#define RETURN_VENDOR(str)        \
    if (strcmp(vendor, str) == 0) \
        return str;

    RETURN_VENDOR(VENDOR_OLDAMD);
    RETURN_VENDOR(VENDOR_AMD);
    RETURN_VENDOR(VENDOR_INTEL);
    RETURN_VENDOR(VENDOR_VIA);
    RETURN_VENDOR(VENDOR_OLDTRANSMETA);
    RETURN_VENDOR(VENDOR_TRANSMETA);
    RETURN_VENDOR(VENDOR_CYRIX);
    RETURN_VENDOR(VENDOR_CENTAUR);
    RETURN_VENDOR(VENDOR_NEXGEN);
    RETURN_VENDOR(VENDOR_UMC);
    RETURN_VENDOR(VENDOR_SIS);
    RETURN_VENDOR(VENDOR_NSC);
    RETURN_VENDOR(VENDOR_RISE);
    RETURN_VENDOR(VENDOR_VORTEX);
    RETURN_VENDOR(VENDOR_OLDAO486);
    RETURN_VENDOR(VENDOR_AO486);
    RETURN_VENDOR(VENDOR_ZHAOXIN);
    RETURN_VENDOR(VENDOR_HYGON);
    RETURN_VENDOR(VENDOR_ELBRUS);
    RETURN_VENDOR(VENDOR_QEMU);
    RETURN_VENDOR(VENDOR_KVM);
    RETURN_VENDOR(VENDOR_VMWARE);
    RETURN_VENDOR(VENDOR_VIRTUALBOX);
    RETURN_VENDOR(VENDOR_XEN);
    RETURN_VENDOR(VENDOR_HYPERV);
    RETURN_VENDOR(VENDOR_PARALLELS);
    RETURN_VENDOR(VENDOR_PARALLELS_ALT);
    RETURN_VENDOR(VENDOR_BHYVE);
    RETURN_VENDOR(VENDOR_QNX);
#undef RETURN_VENDOR

    return "Unknown";
}

bool CPUID::has_feature(CPUFeature feature)
{
    // eax (function) = 1
    // read from both ecx and edx
    get(CPUIDRequest::GET_FEATURES);

#define FEATURE(feat, reg, bit) \
    case CPUFeature::feat:      \
        return (reg & (1 << bit)) != 0;

    switch (feature) {
        // Features in EDX
        FEATURE(FPU, m_edx, 0)
        FEATURE(VME, m_edx, 1)
        FEATURE(DE, m_edx, 2)
        FEATURE(PSE, m_edx, 3)
        FEATURE(TSC, m_edx, 4)
        FEATURE(MSR, m_edx, 5)
        FEATURE(PAE, m_edx, 6)
        FEATURE(MCE, m_edx, 7)
        FEATURE(CX8, m_edx, 8)
        FEATURE(APIC, m_edx, 9)
        FEATURE(SEP, m_edx, 11)
        FEATURE(MTRR, m_edx, 12)
        FEATURE(PGE, m_edx, 13)
        FEATURE(MCA, m_edx, 14)
        FEATURE(CMOV, m_edx, 15)
        FEATURE(PAT, m_edx, 16)
        FEATURE(PSE36, m_edx, 17)
        FEATURE(PSN, m_edx, 18)
        FEATURE(CLFLUSH, m_edx, 19)
        FEATURE(DS, m_edx, 21)
        FEATURE(ACPI, m_edx, 22)
        FEATURE(MMX, m_edx, 23)
        FEATURE(FXSR, m_edx, 24)
        FEATURE(SSE, m_edx, 25)
        FEATURE(SSE2, m_edx, 26)
        FEATURE(SS, m_edx, 27)
        FEATURE(HTT, m_edx, 28)
        FEATURE(TM, m_edx, 29)
        FEATURE(IA64, m_edx, 30)
        FEATURE(PBE, m_edx, 31)
        // Features in ECX
        FEATURE(SSE3, m_ecx, 0)
        FEATURE(PCLMUL, m_ecx, 1)
        FEATURE(DTES64, m_ecx, 2)
        FEATURE(MONITOR, m_ecx, 3)
        FEATURE(DS_CPL, m_ecx, 4)
        FEATURE(VMX, m_ecx, 5)
        FEATURE(SMX, m_ecx, 6)
        FEATURE(EST, m_ecx, 7)
        FEATURE(TM2, m_ecx, 8)
        FEATURE(SSSE3, m_ecx, 9)
        FEATURE(CID, m_ecx, 10)
        FEATURE(SDBG, m_ecx, 11)
        FEATURE(FMA, m_ecx, 12)
        FEATURE(CX16, m_ecx, 13)
        FEATURE(XTPR, m_ecx, 14)
        FEATURE(PDCM, m_ecx, 15)
        FEATURE(PCID, m_ecx, 17)
        FEATURE(DCA, m_ecx, 18)
        FEATURE(SSE4_1, m_ecx, 19)
        FEATURE(SSE4_2, m_ecx, 20)
        FEATURE(X2APIC, m_ecx, 21)
        FEATURE(MOVBE, m_ecx, 22)
        FEATURE(POPCNT, m_ecx, 23)
        FEATURE(TSC_DEADLINE, m_ecx, 24)
        FEATURE(AES, m_ecx, 25)
        FEATURE(XSAVE, m_ecx, 26)
        FEATURE(OSXSAVE, m_ecx, 27)
        FEATURE(AVX, m_ecx, 28)
        FEATURE(F16C, m_ecx, 29)
        FEATURE(RDRAND, m_ecx, 30)
        FEATURE(HYPERVISOR, m_ecx, 31)
    default:
        return false;
    }
#undef FEATURE
}
}
