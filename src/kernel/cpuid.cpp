#include <kernel/cpuid.h>
#include <kernel/util/asm.h>
#include <kernel/util/kprintf.h>

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

static int strcmp(const char* a, const char* b)
{
    while (*a == *b)
        if (*a == '\0')
            return 0;
        else
            a++;
    return *a - *b;
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

    kprintf("CPU vendor: %s\n", vendor);

    // TODO: For this to properly work, we first need some sort of kstring
    return nullptr;
}

bool CPUID::has_feature(CPUFeature feature)
{
    // eax (function) = 1
    // read from both ecx and edx
    get(CPUIDRequest::GET_FEATURES);
    auto ecx = m_ecx;
    auto edx = m_edx;
    switch (feature) {
        // Features in EDX
    case CPUFeature::FPU:
        return (edx & (1 << 0)) != 0;
    case CPUFeature::VME:
        return (edx & (1 << 1)) != 0;
    case CPUFeature::DE:
        return (edx & (1 << 2)) != 0;
    case CPUFeature::PSE:
        return (edx & (1 << 3)) != 0;
    case CPUFeature::TSC:
        return (edx & (1 << 4)) != 0;
    case CPUFeature::MSR:
        return (edx & (1 << 5)) != 0;
    case CPUFeature::PAE:
        return (edx & (1 << 6)) != 0;
    case CPUFeature::MCE:
        return (edx & (1 << 7)) != 0;
    case CPUFeature::CX8:
        return (edx & (1 << 8)) != 0;
    case CPUFeature::APIC:
        return (edx & (1 << 9)) != 0;
    case CPUFeature::SEP:
        return (edx & (1 << 11)) != 0;
    case CPUFeature::MTRR:
        return (edx & (1 << 12)) != 0;
    case CPUFeature::PGE:
        return (edx & (1 << 13)) != 0;
    case CPUFeature::MCA:
        return (edx & (1 << 14)) != 0;
    case CPUFeature::CMOV:
        return (edx & (1 << 15)) != 0;
    case CPUFeature::PAT:
        return (edx & (1 << 16)) != 0;
    case CPUFeature::PSE36:
        return (edx & (1 << 17)) != 0;
    case CPUFeature::PSN:
        return (edx & (1 << 18)) != 0;
    case CPUFeature::CLFLUSH:
        return (edx & (1 << 19)) != 0;
    case CPUFeature::DS:
        return (edx & (1 << 21)) != 0;
    case CPUFeature::ACPI:
        return (edx & (1 << 22)) != 0;
    case CPUFeature::MMX:
        return (edx & (1 << 23)) != 0;
    case CPUFeature::FXSR:
        return (edx & (1 << 24)) != 0;
    case CPUFeature::SSE:
        return (edx & (1 << 25)) != 0;
    case CPUFeature::SSE2:
        return (edx & (1 << 26)) != 0;
    case CPUFeature::SS:
        return (edx & (1 << 27)) != 0;
    case CPUFeature::HTT:
        return (edx & (1 << 28)) != 0;
    case CPUFeature::TM:
        return (edx & (1 << 29)) != 0;
    case CPUFeature::IA64:
        return (edx & (1 << 30)) != 0;
    case CPUFeature::PBE:
        return (edx & (1 << 31)) != 0;
        // Features in ECX
    case CPUFeature::SSE3:
        return (ecx & (1 << 0)) != 0;
    case CPUFeature::PCLMUL:
        return (ecx & (1 << 1)) != 0;
    case CPUFeature::DTES64:
        return (ecx & (1 << 2)) != 0;
    case CPUFeature::MONITOR:
        return (ecx & (1 << 3)) != 0;
    case CPUFeature::DS_CPL:
        return (ecx & (1 << 4)) != 0;
    case CPUFeature::VMX:
        return (ecx & (1 << 5)) != 0;
    case CPUFeature::SMX:
        return (ecx & (1 << 6)) != 0;
    case CPUFeature::EST:
        return (ecx & (1 << 7)) != 0;
    case CPUFeature::TM2:
        return (ecx & (1 << 8)) != 0;
    case CPUFeature::SSSE3:
        return (ecx & (1 << 9)) != 0;
    case CPUFeature::CID:
        return (ecx & (1 << 10)) != 0;
    case CPUFeature::SDBG:
        return (ecx & (1 << 11)) != 0;
    case CPUFeature::FMA:
        return (ecx & (1 << 12)) != 0;
    case CPUFeature::CX16:
        return (ecx & (1 << 13)) != 0;
    case CPUFeature::XTPR:
        return (ecx & (1 << 14)) != 0;
    case CPUFeature::PDCM:
        return (ecx & (1 << 15)) != 0;
    case CPUFeature::PCID:
        return (ecx & (1 << 17)) != 0;
    case CPUFeature::DCA:
        return (ecx & (1 << 18)) != 0;
    case CPUFeature::SSE4_1:
        return (ecx & (1 << 19)) != 0;
    case CPUFeature::SSE4_2:
        return (ecx & (1 << 20)) != 0;
    case CPUFeature::X2APIC:
        return (ecx & (1 << 21)) != 0;
    case CPUFeature::MOVBE:
        return (ecx & (1 << 22)) != 0;
    case CPUFeature::POPCNT:
        return (ecx & (1 << 23)) != 0;
    case CPUFeature::TSC_DEADLINE:
        return (ecx & (1 << 24)) != 0;
    case CPUFeature::AES:
        return (ecx & (1 << 25)) != 0;
    case CPUFeature::XSAVE:
        return (ecx & (1 << 26)) != 0;
    case CPUFeature::OSXSAVE:
        return (ecx & (1 << 27)) != 0;
    case CPUFeature::AVX:
        return (ecx & (1 << 28)) != 0;
    case CPUFeature::F16C:
        return (ecx & (1 << 29)) != 0;
    case CPUFeature::RDRAND:
        return (ecx & (1 << 30)) != 0;
    case CPUFeature::HYPERVISOR:
        return (ecx & (1 << 31)) != 0;
    default:
        return false;
    }
    return false;
}

}
