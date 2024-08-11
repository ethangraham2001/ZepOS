#include <stdio.h>
#include <stdint.h>

#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <kernel/phys_mem_manager.h>
#include <stdlib.h>

extern char _kernel_start;
extern char _kernel_end;

// this is the main entry-point for the kernel woohoo!
void kmain(multiboot_info_t *mbi, unsigned int magic)
{
	terminal_initialize();

	printf("kernel start: %x\n", &_kernel_start);
	printf("kernel end:   %x\n", KERN_PHYSICAL_ADDRESS(&_kernel_end));

	// account for higher-half kernel mapping in virtual memory
	mbi = (multiboot_info_t *)((uint32_t)mbi + KERNEL_VIRTUAL_BASE);

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		abort();
	}
	if (!(mbi->flags >> 6 & 0x1)) {
		abort();
	}

	pmm_init(mbi);

	phys_frame_num_t frame_num = pmm_alloc_frame();
	printf("allocated page starting at %x\n", frame_num_to_addr(frame_num));
}
