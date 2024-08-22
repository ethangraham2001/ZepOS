#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

// initializes the global descriptor table
void gdt_init();

/**
 * defines the fields in a gdt entry
 */
struct __attribute__((packed)) gdt_entry {
	uint32_t limit;
	uint32_t base;
	uint8_t access_byte;
	uint8_t flags;
};

#endif
