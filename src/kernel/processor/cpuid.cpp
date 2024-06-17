#include <kernel/processor/cpuid.h>

#include <libc/string.h>

namespace Kernel {

// CPU vendor strings
static constexpr StringView VENDOR_OLDAMD = "AMDisbetter!";
static constexpr StringView VENDOR_AMD = "AuthenticAMD";
static constexpr StringView VENDOR_INTEL = "GenuineIntel";
static constexpr StringView VENDOR_VIA = "VIA VIA VIA ";
static constexpr StringView VENDOR_OLDTRANSMETA = "TransmetaCPU";
static constexpr StringView VENDOR_TRANSMETA = "GenuineTMx86";
static constexpr StringView VENDOR_CYRIX = "CyrixInstead";
static constexpr StringView VENDOR_CENTAUR = "CentaurHauls";
static constexpr StringView VENDOR_NEXGEN = "NexGenDriven";
static constexpr StringView VENDOR_UMC = "UMC UMC UMC ";
static constexpr StringView VENDOR_SIS = "SiS SiS SiS ";
static constexpr StringView VENDOR_NSC = "Geode by NSC";
static constexpr StringView VENDOR_RISE = "RiseRiseRise";
static constexpr StringView VENDOR_VORTEX = "Vortex86 SoC";
static constexpr StringView VENDOR_OLDAO486 = "GenuineAO486";
static constexpr StringView VENDOR_AO486 = "MiSTer AO486";
static constexpr StringView VENDOR_ZHAOXIN = "  Shanghai  ";
static constexpr StringView VENDOR_HYGON = "HygonGenuine";
static constexpr StringView VENDOR_ELBRUS = "E2K MACHINE ";
// Hypervisors
static constexpr StringView VENDOR_QEMU = "TCGTCGTCGTCG";
static constexpr StringView VENDOR_KVM = " KVMKVMKVM  ";
static constexpr StringView VENDOR_VMWARE = "VMwareVMware";
static constexpr StringView VENDOR_VIRTUALBOX = "VBoxVBoxVBox";
static constexpr StringView VENDOR_XEN = "XenVMMXenVMM";
static constexpr StringView VENDOR_HYPERV = "Microsoft Hv";
static constexpr StringView VENDOR_PARALLELS = " prl hyperv ";
static constexpr StringView VENDOR_PARALLELS_ALT = " lrpepyh vr ";
static constexpr StringView VENDOR_BHYVE = "bhyve bhyve ";
static constexpr StringView VENDOR_QNX = " QNXQVMBSQG ";

void CPUID::get(CPUIDRequest request, u32 ecx) {
    asm volatile("cpuid"
                 : "=a"(m_eax), "=b"(m_ebx), "=c"(m_ecx), "=d"(m_edx)
                 : "a"(request), "c"(ecx));
}

StringView CPUID::vendor() {
    get(CPUIDRequest::GET_VENDOR_STRING);

    char vendor[13];
    for (auto i = 0; i < 4; i++)
        vendor[i] = static_cast<char>(m_ebx >> (i * 8));
    for (auto i = 0; i < 4; i++)
        vendor[i + 4] = static_cast<char>(m_edx >> (i * 8));
    for (auto i = 0; i < 4; i++)
        vendor[i + 8] = static_cast<char>(m_ecx >> (i * 8));
    vendor[12] = '\0';

#define RETURN_VENDOR(str)               \
    if (strcmp(vendor, str.data()) == 0) \
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

bool CPUID::has_feature(CPUFeature feature) {
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

ProcessorInfo CPUID::info() {
    get(CPUIDRequest::GET_FEATURES);

    ProcessorInfo info {
        .stepping = m_eax & 0x0F,
        .model = (m_eax >> 4) & 0x0F,
        .family = (m_eax >> 8) & 0x0F,
        .type = static_cast<ProcessorType>((m_eax >> 12) & 0x03),
    };
    const auto extended_model = (m_eax >> 16) & 0x0F;
    const auto extended_family = (m_eax >> 20) & 0xFF;

    switch (info.family) {
    case 6:
        info.model += extended_model << 4;
        break;
    case 15:
        info.family += extended_family;
        info.model += extended_model << 4;
        break;
    default:
        break;
    }

    return info;
}

}
