# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:
.align 4096
# Reserve page directory and page table
page_directory:
.skip 4096
page_table:
.skip 4096

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

    call setup_paging

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start

setup_paging:
    # clear the page directory
    movl $page_directory, %edi  # page_directory address in %edi
    xorl %eax, %eax             # zero out %eax
    movl $1024, %ecx            # set counter to 1024 (so we loop 1024 times)
    rep stosl

    # set up identity mapping for the first four megabytes
    movl $0x0003, %eax          # set %eax to 3 = (Present, R/W)
    movl $page_directory, %edi  # move page directory addr into edi
    movl %eax, %edi             # store eax in edi

    movl $page_table, %edi
    movl $1024, %edx            # set counter to 1024
1:  # loop label
    movl %eax, %edi             # store eax in page table entry at edi
    addl $4096, %eax            # we increment by the size of a page
    addl $4, %edi               # increment edi by one word (next entry)
    loop 1b

    call enable_paging

# Write into registers to enable paging
enable_paging:
    # move page directory address into cr3
    movl $page_directory, %eax
    movl %eax, %cr3

    # set paging (PG) and protection (PE) bits of cr0
    movl %cr0, %eax
    orl $0x80000001, %eax
    mov %eax, %cr0
    ret
