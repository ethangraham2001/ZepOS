#include <kernel/phys_mem_manager.h>
#include <kernel/util.h>
#include <stdint.h>
#include <stdio.h>

extern char _kernel_start[];
extern char _kernel_end[];

extern char _pmm_start[];
extern char _pmm_end[];

static pmm_data_t pmm __attribute__((section(".pmm_data"))) = { 0 };

// after the first 4MiB identity mapped region of memory
static uint32_t _alloc_start = 0x400000;

// frees the frame at `frame_num`
static inline void pmm_free_frame(uint32_t frame_num);
// allocates the frame at `frame_num`
static inline void pmm_alloc_frame(uint32_t frame_num);

int pmm_init(multiboot_info_t *mbi)
{
	uint32_t i;
	for (i = 0; i < mbi->mmap_length; i += sizeof(multiboot_memory_map_t)) {
		multiboot_memory_map_t *mmmt =
			(multiboot_memory_map_t *)(mbi->mmap_addr + i);
		mmmt = KERN_VIRTUAL_ADDRESS(
			mmmt); // required since paging is enabled

		if (mmmt->type != MULTIBOOT_MEMORY_AVAILABLE)
			continue;

		uint32_t region_start = mmmt->addr_lo;
		uint32_t region_end = region_start + mmmt->len_lo;

		if (region_end < _alloc_start)
			continue;

		// 4KiB-align the region_start
		uint32_t curr_addr = (region_start + PAGE_SIZE - 1) &
				     ~(PAGE_SIZE - 1);
		while (curr_addr < region_end) {
			if (curr_addr >= _alloc_start) {
				uint32_t page_num = curr_addr / PAGE_SIZE;
				pmm_free_frame(page_num);
				pmm.num_free_frames++;
			}

			curr_addr += PAGE_SIZE;
		}
	}

	return 0;
}

uint32_t pmm_get_num_free_frames()
{
	return pmm.num_free_frames;
}

static inline void pmm_free_frame(uint32_t frame_num)
{
	uint32_t entry_num = frame_num / 8;
	uint8_t mask = 1 << (frame_num % 8);
	pmm.frame_bitmap[entry_num] |= mask;
}

static inline void pmm_alloc_frame(uint32_t frame_num)
{
	uint32_t entry_num = frame_num / 8;
	uint8_t mask = 0xFF;
	mask ^= 1 << (frame_num % 8);
	pmm.frame_bitmap[entry_num] |= mask;
}
