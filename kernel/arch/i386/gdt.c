#include <kernel/gdt.h>
#include <stdint.h>
#include <stdlib.h>

extern uint64_t _gdt_start[];
extern uint64_t _gdt_end[];

static void set_gdt_entry(int entry_num, struct gdt_entry source);

static struct gdt_entry kernel_code_seg = {
	.limit	= 0xFFFFF,
	.base	= 0,
	.access_byte = 0x00,
	.flags	= 0xC,
};

void gdt_init()
{
	// the 0th entry should always be the null descriptor
	_gdt_start[0] = 0x0;
	set_gdt_entry(1, kernel_code_seg);
}

static void set_gdt_entry(int entry_num, const struct gdt_entry source)
{
	if (_gdt_start + entry_num > _gdt_end || entry_num == 0)
		panic("invalid entry number for gdt entry");

	if (source.limit > 0xFFFFF)
		panic("gdt entry limit should not exceed 0xFFFFF (20 bits)");

	char *target = (char *)(_gdt_start + entry_num);

	target[0] = source.limit & 0xFF;
	target[1] = (source.limit >> 8) & 0xFF;
	target[6] = (source.limit >> 16) & 0xF;

	target[2] = source.base & 0xFF;
	target[3] = (source.base >> 8) & 0xFF;
	target[4] = (source.base >> 16) & 0xFF;
	target[7] = (source.base >> 24) & 0xFF;

	target[5] = source.access_byte;
	target[6] |= (source.flags & 0xF) << 4;
}
