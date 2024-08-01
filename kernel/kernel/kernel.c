#include <stdio.h>

#include <kernel/tty.h>

// this is the main entry-point for the kernel woohoo!
void kernel_main(void) {
	terminal_initialize();
	printf("printing integer: %d\n", 2311);
	printf("printing character: %c\n", 'u');
	printf("printing string: %s\n", "Hello, world!");
	printf("printing hex: %x\n", 0xFFFF);
	int tmp = printf("printing hex: %x\n", 0x2A3B);
	printf("prev printf returned %d", tmp);
}
