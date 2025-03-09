#ifndef __DENTON_STDDEF_H
#define __DENTON_STDDEF_H

#include <stddef.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/* Macro black-magic
 *
 * The end result of this macro is that it can be used in the following fashion:
 *
 * using_cond(cond, cmd1, cmd2) {
 *    ... Code ...
 * } else {
 *    ... Error handling ...
 * }
 *
 * Where the condition 'cond' is check to be 'true' (non-zero), and if that is
 * the case 'cmd1' will be executed. Then, at the end of the using_cond()
 * block, 'cmd2' will be executed. Both commands will only execute once, and
 * neither command will be executed in the event that 'cond' is false.
 * More-over, the 'else' block will be run if 'cond' fails, and the using_cond
 * block won't be executed.
 *
 * This is mostly useful for locking situations, where you can take a lock, run
 * the code inside of the using_cond block, and then release the lock. The nice
 * part of this is that for simple blocks like this you can't forget to release
 * the lock because the release is part of the using_cond statement.
 *
 * Another nicety, 'break'  and 'continue' will both exit the using_cond *and*
 * still release the locks. 'return' will *not* release the lock though, so
 * return should basically never be used inside a using_cond.
 *
 * The actual macro itself isn't *too* complicated, just follow the 'goto's
 * through the code, just fairly messy. The key to reading this code is that
 * most of it will be removed by the compiler, and a lot of the blocks are
 * simply used to allow us to jump to another section of code. The reason it is
 * so weird looking is so it works as a prefix statement. IE. You can use it
 * with or without a block, and it doesn't require any 'using_end' macro after
 * its usage.
 */
#define using_cond_ctr(cond, cmd1, cmd2, ctr)                     \
	while (1)                                                      \
	if (0)                                                         \
		__PASTE(__using_finished, ctr):                                 \
		break;                                                     \
	else                                                           \
		for (int __using_cond = 0;;)                               \
			if (1)                                                 \
				goto __PASTE(__using_body_init, ctr);                   \
			else                                                   \
				while (1)                                          \
					while (1)                                      \
						if (1) {                                   \
							if (__using_cond)                      \
								cmd2;                              \
							goto __PASTE(__using_finished, ctr);        \
							__PASTE(__using_body_init, ctr):            \
							__using_cond = (cond);                 \
							if (__using_cond)                      \
								cmd1;                              \
							goto __PASTE(__using_body, ctr);            \
						} else                                     \
							__PASTE(__using_body, ctr):                 \
							if (__using_cond)

#define using_cond(cond, cmd1, cmd2) \
	using_cond_ctr(cond, cmd1, cmd2, __COUNTER__)

/*
 * This one works almost identical to the above using_cond. The different is
 * that this one makes use of the __cleanup attribute from GCC. This bring the
 * *huge* advantage that cmd2 will be called regardless of how the scope is
 * exited. IE. return and goto both trigger cmd2 to run.
 *
 * The small disadvantage is that you need to write a wrapper function for
 * __cleanup which takes a pointer to the argument (due to the way __cleanup
 * works).
 *
 */
#define scoped_using_cond_ctr(cond, cmd1, cmd2, arg, ctr) \
	if (0) { \
		__PASTE(__using_finished, ctr):; \
	} else \
		if (1) { \
			const int __using_cond = (cond); \
			if (!__using_cond) \
				goto __PASTE(__using_finished, ctr); \
			goto __PASTE(__using_temp_declare, ctr); \
		} else \
			__PASTE(__using_temp_declare, ctr): \
			for (typeof(arg) __using_temp __used __cleanup(cmd2) = arg;;) \
				if (1) { \
					cmd1(arg); \
					goto __PASTE(__using_body, ctr); \
				} else \
					while (1) \
						while (1) \
							if (1) { \
								goto __PASTE(__using_finished, ctr); \
							} else \
								__PASTE(__using_body, ctr):

#define scoped_using_assign_ctr(cmd1, cmd2, arg, init, ctr) \
	if (0) { \
		__PASTE(__using_finished, ctr):; \
	} else \
		for (typeof(arg) __using_temp __cleanup(cmd2) = init;;) \
			if (1) { \
				__using_temp = (cmd1); \
				(arg) = __using_temp; \
				goto __PASTE(__using_body, ctr); \
			} else \
				while (1) \
					while (1) \
						if (1) { \
							goto __PASTE(__using_finished, ctr); \
						} else \
							__PASTE(__using_body, ctr):

#define scoped_using_cond(cond, cmd1, cmd2, arg) \
	scoped_using_cond_ctr(cond, cmd1, cmd2, arg, __COUNTER__)

#define scoped_using_assign(cmd1, cmd2, arg, init) \
	scoped_using_assign_ctr(cmd1, cmd2, arg, init, __COUNTER__)

/* The 'nocheck' version doesn't take a condition, just two commands to run.
 *
 * As a note, gcc is smart enough to turn this usage into two direct calls to
 * 'cmd1' and 'cmd2' at the beginning and end of the block, without any extra
 * code or variables. */
#define using_nocheck(cmd1, cmd2) using_cond(1, cmd1, cmd2)

/* The normal 'using' one uses the result of 'cmd1' to decide whether or not to
 * run the code. Used in the event the command you would use as 'cmd1' returns
 * an error code you want to check before you keep going. */
#define using(cmd1, cmd2) using_cond(cmd1, do { ; } while (0), cmd2)

#endif
