#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

#define NUM_GDT_ENTRIES 6

/**
 * initializes the global descriptor table (GDT)
 */
void gdt_init();

/**
 * defines the fields in a GDT entry
 */
struct __attribute__((packed)) gdt_entry {
	uint32_t limit;
	uint32_t base;
	uint8_t access_byte;
	uint8_t flags;
};

/**
 * defines the layout of the GDTR which is loaded into the GDTR register, which
 * is 48 bits in width
 */
struct __attribute__((packed)) gdtr {
	uint16_t limit; // the size of the GDT in bytes
	uint32_t base; // points to the start of the GDT
};

#endif
