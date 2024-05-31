MEMORY
{
    program_memory (rwx) : ORIGIN = 0x00000000, LENGTH = 8192
    data_memory (rwx) : ORIGIN = 0x00000000, LENGTH = 8192
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
