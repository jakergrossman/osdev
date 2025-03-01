#ifndef __DENTON_ARCH_I386_ASM_HALT_H
#define __DENTON_ARCH_I386_ASM_HALT_H

static inline void cli(void)
{
    asm volatile ( "cli" );
}

static inline void hlt(void)
{
    asm volatile ( "hlt" );
}

#endif
