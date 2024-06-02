ENTRY(_start)

MEMORY
{
    IMEM (rx) : ORIGIN = 0x80000000, LENGTH = 64K
    DMEM (rw) : ORIGIN = 0x90000000, LENGTH = 64K
}

SECTIONS
{
    . = ORIGIN(IMEM);

    .text : {
        _start = .;
        *(.text)
        *(.text*)
        _etext = .;  /* End of code section */
    } > IMEM

    .rodata : {
        *(.rodata)
        *(.rodata*)
    } > IMEM

    . = ORIGIN(DMEM);

    .data : {
        _data = .;
        *(.data)
        *(.data*)
        _edata = .;  /* End of initialized data */
    } > DMEM

    .bss : {
        _bss = .;
        *(.bss)
        *(.bss*)
        _ebss = .;  /* End of uninitialized data */
    } > DMEM

    /DISCARD/ : {
        *(.note.GNU-stack)
    }
}
