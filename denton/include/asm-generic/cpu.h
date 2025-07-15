#ifndef __ASM_GENERIC_CPU
#define __ASM_GENERIC_CPU

struct cpu_info;

void cpu_early_init(void);

struct cpu_info* cpu_get_local(void);

void cpu_relax(void);
void cpu_preempt_disable(void);
void cpu_preempt_enable(void);

/** stop execution on the current CPU until an interrupt arrives */
void cpu_halt(void);

/** disable interrupts on the current CPU and spin, forever */
void cpu_stop(void);

#endif
