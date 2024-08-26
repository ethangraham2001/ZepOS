#include <kernel/idt.h>
#include <stdio.h>
#include <stdlib.h>

extern uint64_t _idt_start[];
uint64_t idt[NUM_IDT_ENTRIES] __attribute__((section(".idt")));

volatile int num_interrupts = 0;

/**
 * encodes `source` in-place at the `entry_num`th entry in the IDT
 */
static void set_idt_entry(int entry_num, struct idt_entry source);

__attribute__((interrupt)) void default_isr(struct interrupt_frame *frame)
{
	// Disable interrupts to prevent nested interrupts
	asm volatile("cli");

	printf("Interrupt! eip: %x, cs: %x, flags: %x\n", frame->eip, frame->cs,
	       frame->flags);

	// Send EOI (End of Interrupt) to the PIC which is necessary for hardware
	// interrupts
	asm volatile("movb $0x20, %al");
	asm volatile("outb %al, $0x20");

	// Re-enable interrupts
	asm volatile("sti");
}

struct idt_entry default_entry = {
	.offset = (uint32_t)default_isr,
	.segment_selector = 0x08,
	.gate_type = 0x8E, // Present, Ring 0, 32-bit Interrupt Gate
	.dpl = 0,
};

void idt_init()
{
		// populate the IDT with default entries for now
	for (int i = 0; i < NUM_IDT_ENTRIES; i++)
		set_idt_entry(i, default_entry);

	struct idtr idtr = { .size = sizeof(uint64_t) * NUM_IDT_ENTRIES - 1,
			     .base = (uint32_t)&idt };

	// tell the CPU where to find the IDT by loading it into the IDTR register
	asm volatile("lidt %0" ::"m"(idtr));
}

static void set_idt_entry(int entry_num, struct idt_entry source)
{
	if (entry_num > NUM_IDT_ENTRIES || entry_num < 0)
		panic("invalid idt entry number");

	char *target = (char *)&idt[entry_num];

	target[0] = source.offset & 0xFF;
	target[1] = (source.offset >> 8) & 0xFF;
	target[6] = (source.offset >> 16) & 0xFF;
	target[7] = (source.offset >> 24) & 0xFF;

	target[2] = source.segment_selector & 0xFF;
	target[3] = (source.segment_selector >> 8) & 0xFF;

	target[5] = 0b10000000 | ((source.dpl & 0b11) << 5) |
		    (source.gate_type & 0b1111);
}
