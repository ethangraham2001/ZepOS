#include <stdint.h>
#include <kernel/multiboot.h>

#define KERNEL_VIRTUAL_BASE 0xC0000000

/**
 * TODO this is a bad macro and should be removed once thigns are up and 
 * running properly. Just tired right now. But this is what is already mapped
 * in the page table at boot (i.e. the first 22 bits of the address space)
 */
#define ALREADY_MAPPED_UPPER_BOUND_PHYSICAL 0x400000

/**
 * Converts a physical address `a` into a virtual address for kernel-related
 * things. This is only usable for addresses that are configured in the 
 * bootloader before paging is enabled, such as `multiboot_info_t` and its
 * related structs
 */
#define KERN_VIRTUAL_ADDRESS(a) ((void*)((uint32_t) a + KERNEL_VIRTUAL_BASE))

/**
 * Conversely, we want to convert kernel address `a` from a virtual address to
 * a physical one, namely for the physical memory manager
 */
#define KERN_PHYSICAL_ADDRESS(a) ((void*)((uint32_t) a - KERNEL_VIRTUAL_BASE))

// this sits in the kernel
#define PMM_PHYS_ADDRESS 0x110000

// 4KiB
#define PAGE_SIZE 4096
// 4GiB worth of 4KiB pages
#define MAX_PAGES 1048576

/**
 * defines a physical memory manager
 */
typedef struct pmm_data {
	uint32_t num_free_frames;
	uint8_t frame_bitmap[MAX_PAGES / 8];
} pmm_data_t;

typedef int32_t phys_frame_num_t;

typedef uint32_t phys_addr_t;
typedef uint32_t virt_addr_t;

/**
 * Initializes the physical memory from memory data read from the multiboot 
 * info passed at kernel boot
 */
int pmm_init(multiboot_info_t *mbi);

/**
 * returns the number of free frames in the physical memory manager
 */
uint32_t pmm_get_num_free_frames();
