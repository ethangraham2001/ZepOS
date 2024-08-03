#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <stdlib.h>

#define KERNEL_VIRTUAL_BASE 0xC0000000

/**
 * Converts a physical address `a` into a virtual address. This is useful for
 *
 */
#define VIRTUAL_ADDRESS(a) ((void*)((unsigned int) a + KERNEL_VIRTUAL_BASE))

// this is the main entry-point for the kernel woohoo!
void kmain(multiboot_info_t* mbi, unsigned int magic) {
	terminal_initialize();

    // account for higher-half kernel mapping in virtual memory
    mbi = (multiboot_info_t*)((unsigned int)mbi + KERNEL_VIRTUAL_BASE);

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        abort();
    }
    if (!(mbi->flags >> 6 & 0x1)) {
        abort();
    }

    printf("mem_lower %x, mem_upper %x\n\n", mbi->mem_lower, mbi->mem_upper);

    unsigned int i;
    for (i = 0; i < mbi->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t *mmmt = 
            (multiboot_memory_map_t*) (mbi->mmap_addr + i);
        mmmt = VIRTUAL_ADDRESS(mmmt);

        printf("addr_lo %x, addr_hi %x\nlength_lo %x, length_hi %x\n",
            mmmt->addr_lo, mmmt->addr_hi, mmmt->len_lo, mmmt->len_hi);

        if (mmmt->type != MULTIBOOT_MEMORY_AVAILABLE) {
            printf("\tthis block is available\n");
        }
    }
}
