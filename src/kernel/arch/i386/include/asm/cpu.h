#ifndef __DENTON_ARCH_I386_ASM_CPU_H
#define __DENTON_ARCH_I386_ASM_CPU_H

#include <denton/stringify.h>
#include <asm/gdt.h>


struct task;

struct cpu_info {
	int cpu_id;
	int intr_count;

	struct gdt_entry gdt_entries[GDT_ENTRIES];

	struct task* current;

	/* self reference pointer is handy to stuff into per-cpu GDT variable */
	struct cpu_info* self;
};

void cpu_early_init(void);

/*
 * Volatile isn't enough to prevent the compiler from reordering the
 * read/write functions for the control registers and messing everything up.
 * A memory clobber would solve the problem, but would prevent reordering of
 * all loads stores around it, which can hurt performance. Solution is to
 * use a variable and mimic reads and writes to it to enforce serialization
 */
#include <stdint.h>
extern unsigned long __force_order;

/** declare an ordered read function on a control register using volatile asm */
#define __cpu_read_crN_decl(n) \
static inline uint32_t cpu_read_cr##n (void) \
{ \
	uint32_t val = 0; \
	asm volatile ( \
		"mov %%cr" __stringify(n) ", %0" \
		: "=r" (val), "=m" (__force_order) \
	); \
	return val; \
} 

/** declare an ordered write function on a control register using volatile asm */
#define __cpu_write_crN_decl(n) \
static inline void cpu_write_cr##n (uint32_t val) \
{ \
	asm volatile ( \
		"mov %0, %%cr" __stringify(n) \
		: \
		: "r" (val), "m" (__force_order) \
		: "memory" \
	); \
} \

__cpu_write_crN_decl(0);
__cpu_write_crN_decl(1);
__cpu_write_crN_decl(2);
__cpu_write_crN_decl(3);
__cpu_write_crN_decl(4);
__cpu_write_crN_decl(5);
__cpu_write_crN_decl(6);
__cpu_write_crN_decl(7);
__cpu_write_crN_decl(8);

__cpu_read_crN_decl(0);
__cpu_read_crN_decl(1);
__cpu_read_crN_decl(2);
__cpu_read_crN_decl(3);
__cpu_read_crN_decl(4);
__cpu_read_crN_decl(5);
__cpu_read_crN_decl(6);
__cpu_read_crN_decl(7);
__cpu_read_crN_decl(8);

static inline void
arch_cpu_halt(void)
{
	asm volatile (
		"	cli\n"
		"1: jmp 1b\n"
	);
}

static inline struct cpu_info*
cpu_get_local(void)
{
	void* info;
	asm volatile (
		"movl %%gs: 0, %0"
		: "=r" (info)
	);
	return info;
}

#endif
