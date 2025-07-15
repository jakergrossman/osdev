/**
 * unifying interrupt handler:
 * pushes processor state onto the stack, then calls our single IRQ handler
 */
	.globl irq_handler
irq_handler:
	# save registers going in
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushal

	# set segment selectors to our data segment
	# TODO: don't hardcode this
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs

	cld

	pushl %esp
	call irq_global_handler
	addl $4, %esp

irq_handler_end:
	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	addl $0x8, %esp # irq num and error code
	iretl

/** macro definition for 1st stage interrupt handler */
.altmacro
.macro irqN irqno
.type irq\irqno, @function
irq\irqno:
	.if !((\irqno == 8) || (\irqno >= 10 && \irqno <= 14))
		pushl $0
	.endif
	pushl $\irqno
	jmp irq_handler
	iretl
.endm

/** declare irq0-irq255 */
.section .text
.altmacro
.set i,0
.rept 256
	irqN %i
	.set i, i+1
.endr

/** declare array of all IRQ handler addresses */
.macro irq_labelN irqno
	.long irq\irqno
.endm

.globl irq_handlers
irq_handlers:
.set i,0
.rept 256
	irq_labelN %i
	.set i, i+1
.endr
