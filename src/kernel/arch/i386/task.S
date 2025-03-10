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
