ENTRY (_start)

SECTIONS
{
	. = 0x00100000;
	/* The kernel will live at 3GB + 1MB in the virtual address space, */
	/* which will be mapped to 1MB in the physical address space. */
	/* Note that we page-align the sections. Hence 4K! */

	_kernel_start = .;
	.multiboot.data : {
		*(.multiboot.data)
	}

	.multiboot.text : {
		*(.multiboot.text)
   }

   /* here we increment the virtual memory counter by 0xC0000000 */
   . += 0xC0000000;
   /* Add a symbol that indicates the start address of the kernel. 
	* The `AT` indicates that the physical address is placed 0xC0000000 lower! 
	* I.e. the physical location is in the lower memory section. */
	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}
	/* GDT (Global Descriptor Table) section */
	.gdt ALIGN (4K) : AT (ADDR (.gdt) - 0xC0000000)
	{
		_gdt_start = .;
		*(.gdt)
		_gdt_end = .;
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
			*(.bss)
			*(.bootstrap_stack)
	}
	/* Add a symbol that indicates the end address of the kernel. */
	_kernel_end = .;

	. = ALIGN(4096);
	_pmm_start = .;
	/* sections decorated with __attribute__((".pmm_data")) will go here */
	.pmm_data : { *(.pmm_data) }
	_pmm_end = .;
}
