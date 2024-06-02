MEMORY
{
    program_memory (rwx) : ORIGIN = 0x00000000, LENGTH = 32K
    data_memory (rwx) : ORIGIN = 0x00008000, LENGTH = 32K
}

SECTIONS
{
    .text : ALIGN(4)
    {
        *(.text)
    } > program_memory

    .data : ALIGN(4)
    {
        *(.data)
        *(.rodata)
        *(.sdata)
    } > data_memory
}