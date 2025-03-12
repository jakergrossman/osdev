# must match arch_task_context offsets
#define ESP_OFF 0
#define EBI_OFF 4
#define CR3_OFF 8

# void arch_task_switch(struct arch_task_context* old, struct arch_task_context* new)
.global arch_task_switch
.type arch_task_switch, @function
arch_task_switch:
	# 1) eax, ecx, and edx are caller-saved in c-decl
	# 2) the task isn't able to change CR3, so we don't need to save it
	# 3) segment registers are constant in kernel space, so we don't need to save them
	mov 4(%esp), %eax # eax = old
	mov 8(%esp), %edx # edx = new

	push %ebx
	push %esi
	push %edi
	push %ebp

	# save esp back to old task
	mov %esp, ESP_OFF(%eax)

	# load esp for new task
	mov ESP_OFF(%edx), %esp
	# eax = cr3
	mov CR3_OFF(%edx), %eax
	# ebx = address for top of kernel stack
	# mov %ebx ESP0_OFF(__tss)
	
	# only invalidate TLB if we have to
	mov %cr3, %ecx
	cmp %eax, %ecx
	je .same_virtual_space
	mov %eax, %cr3

.same_virtual_space:
	# must be reverse order as above
	pop %ebp
	pop %edi
	pop %esi
	pop %ebx

	ret

.equ TASK_CTX, 0
.equ TASK_EAX, TASK_CTX + 0
.equ TASK_ECX, TASK_CTX + 4
.equ TASK_EDX, TASK_CTX + 8
.equ TASK_EBX, TASK_CTX + 12
.equ TASK_ESP, TASK_CTX + 16
.equ TASK_EBP, TASK_CTX + 20
.equ TASK_ESI, TASK_CTX + 24
.equ TASK_EDI, TASK_CTX + 28
.equ TASK_EIP, TASK_CTX + 32
.equ TASK_EFLAGS, TASK_CTX + 36
.equ TASK_CS, TASK_CTX + 40
.equ TASK_DS, TASK_CTX + 44
.equ TASK_CR3, TASK_CTX + 48
.equ TASK_STACK_TOP, TASK_CTX + 52
.equ TASK_STACK_FRAME_TOP, TASK_CTX + 56
.equ TASK_FPU_STATE, TASK_CTX + 60

NEW_TASK_INIT_EFLAGS = ((0 << 12) | (1 << 9))

/* arch_task_entry(struct task* new_task) */
.global arch_task_entry
.type arch_task_entry, @function
arch_task_entry:
	/* eax = new_task */
	mov 4(%esp), %eax

	/* load esp from context */
	mov TASK_ESP(%eax), %esp

	/* load eflags, CS, and EIP to be
	 * popped when we iret
	 */
	push $NEW_TASK_INIT_EFLAGS
	push TASK_CS(%eax)
	push TASK_EIP(%eax)

	mov TASK_DS(%eax), %ds
	mov TASK_DS(%eax), %es

	iret

/* arch_task_context_restore(struct arch_task_context* ctx) */
.global arch_task_context_restore
.type arch_task_context_restore, @function
arch_task_context_restore:
	/* preserve the current (task switching) context */
	push %ebp
	mov %esp, %ebp

	push %eax

	/* eax = ctx */
	mov 12(%esp), %eax

	mov TASK_EBP(%eax), %ebp
	mov TASK_ESP(%eax), %esp

	/* frstor TASK_FPU_STATE(%eax) */

	mov TASK_EBX(%eax), %ebx
	mov TASK_ECX(%eax), %ecx
	mov TASK_EDX(%eax), %edx
	mov TASK_ESI(%eax), %esi
	mov TASK_EDI(%eax), %edi

	push TASK_EFLAGS(%eax)
	popf

	/* restore task switching context */
	pop %eax
	
	mov %ebp, %esp
	pop %ebp

	ret

/* arch_task_context_save(struct arch_task_context* ctx) */
.global arch_task_context_save
.type arch_task_context_save, @function
arch_task_context_save:
	/* preserve the current (task switching) context */
	push %ebp
	mov %esp, %ebp

	push %eax

	/* eax = ctx */
	mov 12(%esp), %eax

	push %ebx

	mov %ebx, TASK_EBX(%eax)
	mov %ecx, TASK_ECX(%eax)
	mov %edx, TASK_EDX(%eax)
	mov %esi, TASK_ESI(%eax)
	mov %edi, TASK_EDI(%eax)
	mov %esp, TASK_ESP(%eax)
	mov %ebp, TASK_EBP(%eax)

	pushfl
	pop %ebx
	mov %ebx, TASK_EFLAGS(%eax)
	/* fnsave TASK_FPU_STATE(%eax) */

	pop %ebx

	pop %eax

	mov %ebp, %esp
	pop %ebp

	ret
