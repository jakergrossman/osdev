#include <asm/cpu.h>
#include <asm/gdt.h>

#include <limits.h>

static struct cpu_info cpu;

static void cpu_gdt(struct cpu_info * c)
{
	/* we don't really want segmentation, but it's mandatory, so
	 * set it up transparently so all kernel/user code/data sections
	 * span the entire 32-bit memory space
	 */
	const enum gdt_access re = GDTA_EXEC | GDTA_READ;
	const enum gdt_access wr = GDTA_WRITABLE;
	c->gdt_entries[GDT_NULL] = (struct gdt_entry){ 0 };
	c->gdt_entries[GDT_KERNEL_CODE] = GDT_ENTRY(re, 0, ~0, GDT_DPL_KERNEL);
	c->gdt_entries[GDT_KERNEL_DATA] = GDT_ENTRY(wr, 0, ~0, GDT_DPL_KERNEL);
	c->gdt_entries[GDT_USER_CODE]   = GDT_ENTRY(re, 0, ~0, GDT_DPL_USER);
	c->gdt_entries[GDT_USER_DATA]   = GDT_ENTRY(wr, 0, ~0, GDT_DPL_USER);

	/* cpu-local variable */
	c->gdt_entries[GDT_CPU_VAR]
		= GDT_ENTRY(GDTA_WRITABLE, (uintptr_t)&c->self, sizeof(&c->self)-1, GDT_DPL_KERNEL);

	// TODO:
	// c->gdt_entries[GDT_GDT_TSS] = GDT_ENTRY16(GDT_STS_T32A, uint32_t base, uint32_t limit, enum gdt_dpl dpl)

	gdt_flush(c->gdt_entries, 5);
}

void cpu_early_init(void)
{
	cpu_gdt(&cpu);
}
