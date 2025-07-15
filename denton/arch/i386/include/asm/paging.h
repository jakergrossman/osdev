#ifndef __DENTON_ARCH_1386_ASM_PAGING_H
#define __DENTON_ARCH_1386_ASM_PAGING_H

#include <denton/bits.h>
#include <denton/types.h>
#include <stddef.h>
#include <stdint.h>

#include <asm/types.h>

void paging_setup_kernelspace(void);
void paging_set_directory(physaddr_t phys);
void paging_flush_tlb(void);

enum {
	PAGE_SHIFT = 12UL,
	PAGE_SIZE  = (1UL << PAGE_SHIFT),
	PAGE_MASK  = (~(PAGE_SIZE-1)),
	PAGE_ENTRIES = 1024,
};

/* Align to a power of two */
#define ALIGN_2(v, a) ((typeof(v))(((uintptr_t)(v) + (a) - 1) & ~(a - 1)))
#define PAGE_ALIGN(v) ALIGN_2(v, PAGE_SIZE)

#define ALIGN_2_DOWN(v, a) ((typeof(v))(((uintptr_t)(v) & ~(a - 1))))
#define PAGE_ALIGN_DOWN(v) ALIGN_2_DOWN(v, PAGE_SIZE)

typedef union page_direntry {
	uint32_t word;
	struct {
		uint32_t present    : 1;
		uint32_t rw         : 1;
		uint32_t supervisor : 1;
		uint32_t writethru  : 1;
		uint32_t uncached   : 1;
		uint32_t accessed   : 1;
		uint32_t dirty      : 1;
		uint32_t pagesize   : 1;
		uint32_t global     : 1;
		uint32_t reserved   : 3;
		uint32_t frameaddr  : 20;
	} bits;
} page_direntry_t;


typedef union page_entry {
	uint32_t word;
	struct {
		uint32_t present    : 1;
		uint32_t rw         : 1;
		uint32_t supervisor : 1;
		uint32_t writethru  : 1;
		uint32_t uncached   : 1;
		uint32_t accessed   : 1;
		uint32_t dirty      : 1;
		uint32_t pat        : 1;
		uint32_t global     : 1;
		uint32_t reserved   : 3;
		uint32_t frame_addr : 20;
	} bits;
} page_entry_t;

typedef struct page_directory {
	page_direntry_t tables[PAGE_ENTRIES];
} page_directory_t;

typedef struct page_table {
	page_entry_t pages[PAGE_ENTRIES];
} page_table_t;

typedef struct page_bitmask {
	unsigned long words[PAGE_ENTRIES / __BITS_PER_LONG];
} page_bitmask_t;

extern page_directory_t __kernel_pgdir;
extern page_table_t     __kernel_pgtbl;
extern page_bitmask_t   __kernel_bmask;

#define PAGE_DIR_IDX(val) (((val) >> 22) & 0x3FF)
#define PAGE_TBL_IDX(val) (((val) >> 22) & 0x3FF)

#endif
