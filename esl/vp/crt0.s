.section .text
.global _start
.global _exit
.global main

_start:
    # Set up the stack pointer
    la sp, _stack_top

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
