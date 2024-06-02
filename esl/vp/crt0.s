.section .text
.global _start

# Define memory regions
.equ PROGRAM_START, 0x00000000
.equ DATA_START, 0x00008000

# Entry point
_start:
    # Set up the stack pointer (assuming the stack grows downwards)
    la sp, _stack_start

    # Copy initialized data from ROM to RAM
    la t0, _etext
    la t1, _data
    la t2, _edata
copy_data:
    beq t1, t2, call_main
    lw t3, 0(t0)
    sw t3, 0(t1)
    addi t0, t0, 4
    addi t1, t1, 4
    j copy_data

# Call main function
call_main:
    jal ra, main

    # Set exit code (0) and invoke ecall to signal program completion
    li a0, 0
    ecall

# Define the stack start (assuming the stack grows downwards)
.section .data
.align 4
_stack_start:
    .space 4096  # Adjust stack size as needed
