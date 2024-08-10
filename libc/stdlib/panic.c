#include <stdio.h>

__attribute__((__noreturn__)) void panic(const char *reason)
{
#if defined(__is_libk)
	// TODO: Add proper kernel panic.
	printf("kernel panic: %s\n", reason);
#else
	// TODO: Abnormally terminate the process as if by SIGABRT.
	printf("kernel panic: %s\n", reason);
#endif
	while (1) {
	}
	__builtin_unreachable();
}
