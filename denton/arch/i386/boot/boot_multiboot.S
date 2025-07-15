#include <asm/memlayout.h>

# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a multiboot header that marks the program as a kernel.
.section .multiboot.data, "aw"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Allocate the initial stack.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .data
# boot page table, preset to map the first 1M, except for the last entry,
# which maps the VGA buffer
boot_page_table1:
.set addr, 3
.rept 1023
.long addr
.set addr, addr + 0x1000
.endr
.long (0x000B8003)

# boot page directory
.align 4096
boot_page_directory:
.skip 4096

# The kernel entry point.
.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
	movl $(stack_top - __KERNEL_VIRTBASE), %esp

	# multiboot has placed on the stack:
	#   %eax - boot magic
	#   %ebx - physical location of multiboot memory.
	#		   for convenience in cmain, we will place
	#		   the virtual address on the stack, instead
	addl $__KERNEL_VIRTBASE, %ebx
	push %ebx
	push %eax

	# place kernel at 0x0000_0000 (identity map) and virtual base to prep for paging
	movl $(boot_page_table1 - __KERNEL_VIRTBASE + 3), boot_page_directory - __KERNEL_VIRTBASE +   0*4
	movl $(boot_page_table1 - __KERNEL_VIRTBASE + 3), boot_page_directory - __KERNEL_VIRTBASE + 768*4

	# Set cr3 to the address of the boot_page_directory.
	movl $(boot_page_directory - __KERNEL_VIRTBASE), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protect bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump. 
	lea higher_half, %ecx
	jmp *%ecx

.section .text

higher_half:
	# At this point, paging is fully set up and enabled.
	# Correct the stack pointer for the fact we are now running at __KERNEL_VIRTBASE
	addl $__KERNEL_VIRTBASE, %esp

	# Reload crc3 to force a TLB flush so the changes to take effect.
	movl %cr3, %ecx
	movl %ecx, %cr3

	call _init

	# Enter the high-level kernel.
	call cmain

	# Infinite loop if the system has nothing more to do.
	cli
1:	hlt
	jmp 1b
