#ifndef __DENTON_INITCALL_H
#define __DENTON_INITCALL_H

#define __initcall_fn(__name) \
	__init_##__name

#define extern_initcall(__name) void __initcall_fn(__name) (void)

#define initcall(__name, __fn) \
	void __initcall_fn(__name) (void) __attribute__((alias(#__fn)))

#define initcall_depends(init, dep) \
	static void __attribute__((used, section(".discard.initcall.ptrs"))) \
		(*__PASTE(__test_ptr, __COUNTER__))(void) = &__initcall_fn(dep); \
	static void __attribute__((used, section(".discard.initcall.ptrs"))) \
		(*__PASTE(__test_ptr, __COUNTER__))(void) = &__initcall_fn(init); \
	static char __attribute__((used, section(".discard.initcall.deps"), aligned(1))) \
		__PASTE(__dep_info, __COUNTER__)[] = #dep " " #init

/**
 * early     - called immediately before SMP is enabled
 * core      - called immediately after  SMP is enabled
 * subsystem -
 * device    -
 */

#define initcall_early(__name, __fn) \
	initcall(__name, __fn); \
	initcall_depends(__name, early); \
	initcall_depends(core, __name);

#define initcall_core(__name, __fn) \
	initcall(__name, __fn); \
	initcall_depends(__name, core); \
	initcall_depends(subsystem, __name);

#define initcall_subsystem(__name, __fn) \
	initcall(__name, __fn); \
	initcall_depends(__name, subsystem); \
	initcall_depends(device, __name);

#define initcall_device(__name, __fn) \
	initcall(__name, __fn); \
	initcall_depends(__name, device);

#define extern_init

/* constructed by build system */
extern void (*__initcalls[])(void);

extern_initcall(early);
extern_initcall(core);
extern_initcall(subsys);
extern_initcall(device);

#endif
