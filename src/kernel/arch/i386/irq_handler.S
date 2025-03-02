	.globl irq_handler
irq_handler:
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushal

	movw $16, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw $48, %ax
	movw %ax, %gs

	cld

	pushl %esp
	call irq_global_handler
	addl $4, %esp

	.globl irq_handler
irq_handler_end:
	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	addl $0x8, %esp # irq num and error code
	iretl

.altmacro
.macro irqN irqno
irq\irqno:
	.if !((\irqno == 8) || (\irqno >= 10 && \irqno <= 14))
	pushl $0
	.endif
	pushl $\irqno
	jmp irq_handler
.endm

.altmacro
.section .text
.set i,0
.rept 256
	irqN %i
	.set i, i+1
.endr

.macro irq_labelN irqno
	.long irq\irqno
.endm

.globl irq_hands
irq_hands:
.set i,0
.rept 256
	irq_labelN %i
	.set i, i+1
.endr
