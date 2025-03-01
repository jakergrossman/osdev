#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <denton/types.h>
#include <denton/panic.h>
#include <denton/memsize.h>

#include <asm/paging.h>
#include <asm/types.h>
#include <asm/memlayout.h>
#include <asm/cpuid.h>
#include <asm/cpu.h>

page_directory_t __kernel_pgdir __attribute__((aligned(PAGE_SIZE)));
page_table_t     __kernel_pgtbl __attribute__((aligned(PAGE_SIZE)));
page_bitmask_t   __kernel_bmask __attribute__((aligned(PAGE_SIZE)));

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

    uint32_t cr4 = cpu_read_cr4();
    if (cpuid_has_pse()) {
        cr4 |= CPU_CR4_PSE;
    }
    if (cpuid_has_pge()) {
        cr4 |= CPU_CR4_GLB;
    }
    cpu_write_cr4(cr4);

    paging_setup_kernel_pgdir();
    paging_set_directory(v_to_p(&__kernel_pgdir));
    paging_flush_tlb();
}

void paging_set_directory(physaddr_t phys)
{
    cpu_write_cr3(phys);
}

// #include <asm/paging.h>
// #include <uapi/asm/types.h>



// void pga_flush_tlb(void)
// {
//     __cpu_write_cr3(pga_read_cr3());
// }

// struct page_allocator {
//     page_bitmask_t* bmask;
//     page_table_t* table;
// };

// void pga_init(struct page_allocator* const ally)
// {
//     // all pages: unused
//     memset(ally->bmask, 0, sizeof(*ally->bmask));
//     memset(ally->table, 0, sizeof(*ally->table));
// }

// int pga_next_frame_number(struct page_allocator* const ally)
// {
//     unsigned long next_frame = find_first_bit(ally->bmask->words, PAGE_ENTRIES);
//     return (next_frame == PAGE_ENTRIES)
//            ? -1
//            : next_frame;
// }


// page_entry_t* pga_reserve_frame(struct page_allocator* const ally, int frame)
// {
//     set_bit(frame, ally->bmask);
//     return &(ally->table->pages[frame]);
// }

// page_entry_t* pga_alloc_frame(struct page_allocator* const ally)
// {
//     int next = pga_next_frame_number(ally);
//     if (next < 0) {
//         return NULL;
//     }

//     return pga_reserve_frame(ally, next);
// }

// void pga_free_frame(struct page_allocator* const ally, int frame)
// {
//     clr_bit(frame, ally->bmask);
//     ally->table->pages[frame].word = 0;
// }

// struct page_allocator __kernel_pgally = {
//     .table = &__kernel_pgtbl,
//     .bmask = &__kernel_bmask,
// };

// int page_map(page_table_t *const pgdir, void* addressp)
// {
//     uint32_t address = (uint32_t)addressp;
//     size_t dir_index = address >> 22;
//     size_t table_index = (address >> 12) & 0x3FF;

//     page_entry_t* direntry = &pgdir->pages[dir_index];
//     if (!direntry->bits.present) {
//         panic(doesnt exists);
//     }

//     page_table_t* tbl = (typeof(tbl))(direntry->bits.frame_addr);
//     page_entry_t* tblentry = &tbl->pages[table_index];
//     if (tblentry->bits.present) {
//         panic(exists);
//     }

//     page_entry_t effect = {
//         .bits = {
//             .present = true,
//             // TODO
//             .rw = true,
//             .supervisor = true,
//             .frame_addr = ((uint32_t)addressp) & PAGE_MASK,
//         },
//     };
//     *tblentry = effect;
//     pga_flush_tlb();

//     return 0;
// }

// void page_init(void)
// {
//     __cpu_write_cr3((uint32_t)&__kernel_pgdir);

//     // map the last entry of the page directory to itself
//     page_entry_t directory_entry = { .word = (uintptr_t)&__kernel_pgdir };
//     directory_entry.bits.present = true;
//     directory_entry.bits.rw = true;
//     __kernel_pgtbl.pages[PAGE_ENTRIES-1] = directory_entry;

//     // map the kernel to 0xC000_0000
//     page_entry_t kernel_entry = { .word = 0xC0000000 };
//     kernel_entry.bits.present = true;
//     __kernel_pgtbl.pages[0xC0000000 / PAGE_SIZE / 1024] = kernel_entry;

//     // identity map 1M to 2M
//     uint32_t from = __MiX(1);
//     uint32_t base = from / PAGE_SIZE;
//     for (size_t i = base; i < base+__MiX(1)/PAGE_SIZE; i++) {
//         __kernel_pgtbl.pages[i].word = from | 1;
//     }

//     pga_enable_paging();
// }
