#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <denton/types.h>
#include <denton/panic.h>
#include <denton/memsize.h>
#include <denton/klog.h>
#include <denton/tty.h>
#include <denton/mm/bootmem.h>
#include <denton/tty.h>
#include <denton/compiler.h>

#include <asm/paging.h>
#include <asm/types.h>
#include <asm/memlayout.h>
#include <asm/cpuid.h>
#include <asm/cpu.h>

page_directory_t __kernel_pgdir __align(PAGE_SIZE);
page_table_t     __kernel_pgtbl __align(PAGE_SIZE);
page_bitmask_t   __kernel_bmask __align(PAGE_SIZE);

static void paging_setup_kernel_pgdir(void)
{
    page_directory_t* pgdir = &__kernel_pgdir;
    size_t tbl_start = PAGE_DIR_IDX(__KERNEL_VIRTBASE);


    bool pse = cpuid_has_pse();
    bool pge = cpuid_has_pge();

    if (pse) {
        /* use 4MB pages if we can, since they are nicer, and we
         * aren't going to touch this literally ever again
         */
        for (int i = tbl_start; i < PAGE_ENTRIES-1; i++) {
            uintptr_t base = ((i - tbl_start) << 22);

            page_direntry_t entry = { .word = base };
            entry.bits.present = true;
            entry.bits.rw = true;
            entry.bits.pagesize = true;
            entry.bits.global = pge;
            pgdir->tables[i] = entry;
        }
    } else {
        // TODO: use bootmem to allocate this page table pages
        panic();
    }
}

void paging_flush_tlb(void)
{
    cpu_write_cr3(cpu_read_cr3());
}

void paging_setup_kernelspace(void)
{
    bool pse = cpuid_has_pse();
    bool pge = cpuid_has_pge();

    uint32_t cr4 = cpu_read_cr4();
    if (pse) {
        cr4 |= CPU_CR4_PSE;
    }
    if (pge) {
        cr4 |= CPU_CR4_GLB;
    }
    cpu_write_cr4(cr4);

    klog_trace("PSE: %s, PGE: %s\n", pse ? "yes" : "no", pge ? "yes" : "no");

    paging_setup_kernel_pgdir();
    paging_set_directory(v_to_p(&__kernel_pgdir));
    paging_flush_tlb();

	// FIXME:
	terminal_update_base(INIT_VGA);

	klog_trace("page directory at 0x%08X installed\n", v_to_p(&__kernel_pgdir));
}

void paging_set_directory(physaddr_t phys)
{
    cpu_write_cr3(phys);
}
