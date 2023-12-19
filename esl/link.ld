SECTIONS
{
    .text 0 : ALIGN(4)
    {
        *(.text)
    }

    .data 0 : ALIGN(4)
    {
        *(.data)
        *(.rodata)
        *(.sdata)
    }
}