MEMORY
{
    program_memory (rwx) : ORIGIN = 0x00000000, LENGTH = 32K   /* 32 kilobytes for program memory */
    data_memory (rwx) : ORIGIN = 0x00008000, LENGTH = 32K      /* 32 kilobytes for data memory */
}

SECTIONS
{
    .text : ALIGN(4)
    {
        *(.text)
        *(.text*)
        . = ALIGN(4);
        _etext = .;  /* End of text section */
    } > program_memory

    .data : ALIGN(4)
    {
        _data = .;  /* Start of data section */
        *(.data)
        *(.rodata)
        *(.sdata)
        . = ALIGN(4);
        _edata = .;  /* End of data section */
    } > data_memory
}
