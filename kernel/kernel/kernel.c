#include <stdio.h>

#include <kernel/tty.h>

// this is the main entry-point for the kernel woohoo!
void kernel_main(void) {
	terminal_initialize();
	printf("Hello, kernel World! This is ZepOS!\nNewline uwu");
}
