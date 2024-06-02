ENTRY(_start)

MEMORY
{
    IMEM (rx) : ORIGIN = 0x80000000, LENGTH = 64K
    DMEM (rw) : ORIGIN = 0x90000000, LENGTH = 64K
}

SECTIONS
{
    .text : {
        *(.text)
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))
        KEEP(*(.rodata*))
    } > IMEM

    .rodata : {
        *(.rodata)
        *(.rodata*)
    } > IMEM

    .data : {
        *(.data)
        *(.data*)
        KEEP(*(.sdata))
    } > DMEM

    .bss : {
        *(.bss)
        *(.bss*)
        KEEP(*(.sbss))
    } > DMEM

    /DISCARD/ : {
        *(.note.GNU-stack)
    }
}
