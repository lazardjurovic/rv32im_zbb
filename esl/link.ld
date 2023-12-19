/* link.ld */
ENTRY(main)

SECTIONS {
    . = 0;
    .text : { *(.text) }
    .data : { *(.data) }
}
