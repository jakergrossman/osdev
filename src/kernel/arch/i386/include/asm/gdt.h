#ifndef __DENTON_ARCH_1386_ASM_GDT_H
#define __DENTON_ARCH_1386_ASM_GDT_H

/** Global Descriptor Table (IDT) Handling Routines */

#include "denton/compiler.h"
#include <denton/bits/bits.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* descriptor priviledge level */
enum gdt_dpl {
	GDT_DPL_KERNEL = 0x0,
	GDT_DPL_USER = 0x3,
};

enum gdt_access {
	GDTA_EXEC = 0x8,
	GDTA_EXPAND = 0x4,
	GDTA_CODE = 0x4,
	GDTA_WRITABLE = 0x2,
	GDTA_READ = 0x2,
	GDTA_ACCESSED = 0x1,
};

enum gdt_flag {
	GDTF_LONG_MODE = BIT(1),
	GDTF_DB = BIT(2),
	GDTF_GRANULARITY = BIT(3),
};

enum {
	/* GDT type flag for 32-bit TSS */
	GDT_STS_T32A = 0x9,
	GDT_ENTRIES = 7,
};

/* segment IDs */
enum gdt_selector {
	GDT_NULL,
	GDT_KERNEL_CODE,
	GDT_KERNEL_DATA,
	GDT_USER_CODE,
	GDT_USER_DATA,
	GDT_GDT_TSS,
	GDT_CPU_VAR,
};

struct gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access : 4;
	uint8_t desc_type   : 1;
	uint8_t dpl    : 2;
	uint8_t present     : 1;
	uint8_t limit_hi        : 4;
	uint8_t flag      : 4;
	uint8_t base_hi;
} __packed;

struct gdt_ptr {
	uint16_t limit;
	uint32_t base;
} __packed;

/* reconstruct the limit from @gdt */
static inline uint32_t
gdt_limit(const struct gdt_entry* gdt)
{
	return (gdt->limit_low) | (gdt->limit_low << 16);
}

/* reconstruct the base from @gdt */
static inline uint32_t
gdt_base(const struct gdt_entry* gdt)
{
	return (gdt->base_low) | (gdt->base_mid << 16) | (gdt->base_hi << 24);
}

void gdt_flush(const struct gdt_entry* gdt, size_t len);

static __always_inline struct gdt_entry
GDT_ENTRY(enum gdt_access access, uint32_t base, uint32_t limit, enum gdt_dpl dpl)
{
	return (struct gdt_entry) {
		.limit_low = (limit >> 12) & 0xFFFF,
		.limit_hi  = (limit >> 24) & 0x00FF,
		.base_low  = (base  >>  0) & 0xFFFF,
		.base_mid  = (base  >> 16) & 0x00FF,
		.base_hi   = (base  >> 24) & 0x00FF,
		.access    = access,
		.dpl       = dpl,
		.present   = true,
		.desc_type = 1,
		.flag      = GDTF_GRANULARITY | GDTF_DB,
	};
}

static __always_inline struct gdt_entry
GDT_ENTRY16(enum gdt_access access, uint32_t base, uint32_t limit, enum gdt_dpl dpl)
{
	return (struct gdt_entry) {
		.limit_low = (limit) & 0xFFFF,
		.limit_hi  = (limit >> 16) & 0xFFFF,
		.base_low  = (base  >>  0) & 0xFFFF,
		.base_mid  = (base  >> 16) & 0x00FF,
		.base_hi   = (base  >> 24) & 0x00FF,
		.access    = access,
		.dpl       = dpl,
		.present   = true,
		.desc_type = 1,
		.flag      = GDTF_DB,
	};
}


#endif
