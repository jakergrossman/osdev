#include <asm/memlayout.h>

	.set ALIGN, 1<<0
	.set MEMINFO, 1<<1
	.set FLAGS, ALIGN | MEMINFO
	.set MAGIC, 0x1BADB002
	.set CHECKSUM, -(MAGIC + FLAGS)

	.section .multiboot.data, "aw"
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
	
	.section .bootstrap_stack, "aw", @nobits
stack_bottom:
	.skip 16384
stack_top:

	.section .multiboot.text, "a"
	.global _start
	.type _start, @function
_start:
	ldr x30, =stack_top
	mov sp, x30
	bl kmain
	b .
