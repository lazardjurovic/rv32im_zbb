.section .text
.global _start
.global _exit
.global main

_start:
    # Set up the stack pointer
    la sp, _stack_top

    # Copy initialized data from ROM to RAM
    la t0, _etext
    la t1, _data
    la t2, _edata
copy_data:
    beq t1, t2, copy_bss
    lw t3, 0(t0)
    sw t3, 0(t1)
    addi t0, t0, 4
    addi t1, t1, 4
    j copy_data

# Zero initialize the .bss section
copy_bss:
    la t1, _bss
    la t2, _ebss
zero_bss:
    beq t1, t2, call_main
    sw x0, 0(t1)
    addi t1, t1, 4
    j zero_bss

call_main:
    # Call the main function
    call main

    # Move the return value of main into a0 (the argument register for _exit)
    mv a0, a0  # RISC-V ABI convention: a0 holds the return value

    # Jump to the _exit function
    j _exit

_exit:
    # ECALL to signal program completion
    ecall

    # Infinite loop to prevent returning
1:  j 1b

# Define the stack top symbol (set appropriately for your memory layout)
.section .bss
.align 4
_stack_top:
    .space 4096  # Adjust stack size as needed
