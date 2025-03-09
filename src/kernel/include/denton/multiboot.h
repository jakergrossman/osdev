#ifndef __DENTON_MULTIBOOT_H
#define __DENTON_MULTIBOOT_H

#include <stdint.h>

#include <denton/bits.h>

enum {
	MULTIBOOT_HEADER_MAGIC = 0x1BADB002,
	MULTIBOOT_BOOTLOADER_MAGIC = 0x2BADB002, // passed via %eax to _start
};

enum multiboot_flag {
	MBF_INFO_MEMORY = BIT(0),
	MBF_INFO_BOOTDEV = BIT(1),
	MBF_INFO_CMDLINE = BIT(2),
	MBF_INFO_MODS = BIT(3),
	MBF_INFO_AOUT_SYMS = BIT(4),
	MBF_INFO_ELF_SHDR = BIT(5),
	MBF_INFO_MEM_MAP = BIT(6),
	MBF_INFO_DRIVE_INFO = BIT(7),
	MBF_INFO_CONFIG_TABLE = BIT(8),
	MBF_INFO_BOOTLOADER_NAME = BIT(9),
};

struct multiboot_aout_sym_info {
	uint32_t table_size;
	uint32_t str_size;
	uint32_t addr;
	uint32_t reserved;
};

struct multiboot_elf_section_header_table {
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
};

enum multiboot_memmap_type {
	MB_MEMMAP_AVAIL = 1,
	MB_MEMMAP_RESERVED,
	MB_MEMMAP_ACPI_RECLAIMABLE,
	MB_MEMMAP_ACPI_NVS,
	MB_MEMMAP_ACPI_BADRAM,
};

struct multiboot_memmap {
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
} __packed;


struct multiboot_info {
	uint32_t flags;

	uint32_t mem_lower;
	uint32_t mem_upper;

	uint32_t boot_device;

	char*    cmdline;

	uint32_t boot_mod_count;
	uint32_t boot_mod_addr;

	union {
		struct multiboot_aout_sym_info aout_sym;
		struct multiboot_elf_section_header_table elf_sections;
	};

	uint32_t mmap_length;
	uint32_t mmap_addr;

	uint32_t drives_length;
	uint32_t drives_addr;

	uint32_t config_table;

	uint32_t boot_loader_name;
};

#endif
