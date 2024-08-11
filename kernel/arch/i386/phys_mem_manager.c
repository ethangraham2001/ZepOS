#include <kernel/phys_mem_manager.h>
#include <kernel/util.h>
#include <stdint.h>
#include <string.h>

extern char _kernel_start[];
extern char _kernel_end[];

extern char _pmm_start[];
extern char _pmm_end[];

static pmm_data_t pmm __attribute__((section(".pmm_data"))) = { 0 };

// after the first 4MiB identity mapped region of memory
static uint32_t _alloc_start = 0x400000;

// Marks the frame at `frame_num` as free
static inline void _pmm_mark_frame_as_free(uint32_t frame_num);
// Marks the frame at `frame_num` as allocated
static inline void _pmm_mark_frame_as_alloc(uint32_t frame_num);

int pmm_init(multiboot_info_t *mbi)
{
	memset(pmm.frame_bitmap, 0xFF, sizeof(uint8_t) * (MAX_PAGES / 8));
	uint32_t i;
	for (i = 0; i < mbi->mmap_length; i += sizeof(multiboot_memory_map_t)) {
		multiboot_memory_map_t *mmmt =
			(multiboot_memory_map_t *)(mbi->mmap_addr + i);
		mmmt = KERN_VIRTUAL_ADDRESS(mmmt);

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

phys_frame_num_t pmm_alloc_frame()
{
	// scan the bitmap for a free frame
	for (uint32_t i = 0; i < MAX_PAGES / 8; i++) {
		if (pmm.frame_bitmap[i] == 0xFF)
			continue;

		uint8_t entry = pmm.frame_bitmap[i];
		uint32_t j = 0;
		while (entry & 0x1) {
			entry >>= 1;
			j++;
		}

		phys_frame_num_t frame_num = i * 8 + j;
		_pmm_mark_frame_as_alloc(frame_num);
		pmm.num_free_frames--;
		return frame_num;
	}

	return -1;
}

void pmm_free_frame(phys_frame_num_t frame_num)
{
	_pmm_mark_frame_as_free(frame_num);
	pmm.num_free_frames++;
}

uintptr_t frame_num_to_addr(phys_frame_num_t frame_num)
{
	return frame_num * PAGE_SIZE;
}

phys_frame_num_t addr_to_frame_num(uintptr_t addr)
{
	return addr / PAGE_SIZE;
}

static inline void _pmm_mark_frame_as_free(uint32_t frame_num)
{
	uint32_t entry_num = frame_num / 8;
	volatile uint8_t mask = 0xFF;
	mask ^= 1 << (frame_num % 8);
	pmm.frame_bitmap[entry_num] &= mask;
}

static inline void _pmm_mark_frame_as_alloc(uint32_t frame_num)
{
	uint32_t entry_num = frame_num / 8;
	volatile uint8_t mask = 1 << (frame_num % 8);
	pmm.frame_bitmap[entry_num] |= mask;
}
