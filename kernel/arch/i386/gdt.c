#include <kernel/gdt.h>
#include <stdint.h>
#include <stdlib.h>

extern uint64_t _gdt_start[];

uint64_t gdt[NUM_GDT_ENTRIES] __attribute__((section(".gdt")));

/**
 * sets entry `entry_num` as `source` in the GDT
 */
static void set_gdt_entry(int entry_num, struct gdt_entry source);

static struct gdt_entry null_descriptor_seg = {
	.limit = 0xFFFFF,
	.base = 0,
	.access_byte = 0x00,
	.flags = 0x0,
};

static struct gdt_entry kernel_code_seg = {
	.limit = 0xFFFFF,
	.base = 0,
	.access_byte = 0x9A,
	.flags = 0xC,
};

static struct gdt_entry kernel_data_seg = {
	.limit = 0xFFFFF,
	.base = 0,
	.access_byte = 0x92,
	.flags = 0xC,
};

static struct gdt_entry user_code_seg = {
	.limit = 0xFFFFF,
	.base = 0,
	.access_byte = 0xFA,
	.flags = 0xC,
};

static struct gdt_entry user_data_seg = {
	.limit = 0xFFFFF,
	.base = 0,
	.access_byte = 0xF2,
	.flags = 0xC,
};

static struct gdt_entry task_state_seg = {
	.limit = 0x104, // should be size of TSS
	.base = 0,
	.access_byte = 0x92,
	.flags = 0x0,
};

void gdt_init()
{
	// we opt for a 6-segment flat model in protected mode.
	set_gdt_entry(0, null_descriptor_seg);
	set_gdt_entry(1, kernel_code_seg);
	set_gdt_entry(2, kernel_data_seg);
	set_gdt_entry(3, user_code_seg);
	set_gdt_entry(4, user_data_seg);
	set_gdt_entry(5, task_state_seg);

	struct gdtr gdtr = {
		.limit = sizeof(uint64_t) * 6 - 1, // each GDT entry is 8-bytes
		.base = (uint32_t)&gdt,
	};

	// tell the CPU where to find the GDT by loading it into the GDTR
	asm volatile("lgdt %0" ::"m"(gdtr));
}

static void set_gdt_entry(int entry_num, const struct gdt_entry source)
{
	if (entry_num >= NUM_GDT_ENTRIES || entry_num < 0)
		panic("invalid entry number for gdt entry");

	if (source.limit > 0xFFFFF)
		panic("gdt entry limit should not exceed 0xFFFFF (20 bits)");

	char *target = (char *)(&gdt[entry_num]);

	target[0] = source.limit & 0xFF;
	target[1] = (source.limit >> 8) & 0xFF;
	target[6] = (source.limit >> 16) & 0x0F;

	target[2] = source.base & 0xFF;
	target[3] = (source.base >> 8) & 0xFF;
	target[4] = (source.base >> 16) & 0xFF;
	target[7] = (source.base >> 24) & 0xFF;

	target[5] = source.access_byte;
	target[6] |= (source.flags) << 4;
}
