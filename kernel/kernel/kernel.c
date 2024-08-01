#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <stdlib.h>

// this is the main entry-point for the kernel woohoo!
void kmain(multiboot_info_t* mbd, unsigned int magic) {
	terminal_initialize();

    printf("expected magic: %x\n", MULTIBOOT_BOOTLOADER_MAGIC);
    printf("actual magic: %x\n", magic);

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        // kernel panic
        abort();
    }
}
