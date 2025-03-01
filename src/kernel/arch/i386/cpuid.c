#include <asm/cpuid.h>
#include <stdint.h>

uint32_t cpuid_ecx;
uint32_t cpuid_edx;
char cpuid[10];

static void cpuid_get_id(char *cpuid)
{
    asm volatile(
        "cpuid"
        : "=a" (*cpuid),
          "=b" (*(cpuid + 1)),
          "=c" (*(cpuid + 2)),
          "=d" (*(cpuid + 3))
        : "a" (0)
    );
}

static void cpuid_get_feature_flags(uint32_t *ecx_flags, uint32_t *edx_flags)
{
    uint32_t eax = 1, ebx;
    asm volatile(
        "cpuid"
        : "=c" (*ecx_flags),
          "=d" (*edx_flags),
          "=b" (ebx),
          "+a" (eax)
    );
}

void cpuid_init(void)
{
    cpuid_get_id(&cpuid[0]);
    cpuid_get_feature_flags(&cpuid_ecx, &cpuid_edx);
}
