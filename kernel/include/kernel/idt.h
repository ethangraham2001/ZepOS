#ifndef _IDT_H
#define _IDT_H 1

#include <stdint.h>

/**
 * defines the layout of the IDTR, which is to be loaded into the `idtr` 
 * register. It is 48 bits wide
 */
struct __attribute__((packed)) idtr {
	uint16_t size;
	uint32_t base;
};

/**
 * defines the structure of a gate descriptor, which is an entry in the IDT
 */
struct idt_entry {
	uint32_t offset;
	uint16_t segment_selector;
	uint8_t gate_type;
	uint8_t dpl;
};

/**
 * corresponds to the values pushed to stack when an interrupt occurs in IA-32
 */
struct interrupt_frame {
	uint32_t eip;
	uint32_t cs;
	uint32_t flags;
};

#define NUM_IDT_ENTRIES 256

/**
 * initializes the Interrupt Descriptor Table (IDT) with default entries and
 * loads its address into the idtr
 */
void idt_init();

#endif
