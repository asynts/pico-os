/* FIXME: The SDK provides a linker script which seems to discards any section it doesn't understand? */

SECTIONS {
    .rodata : {
        *(.embed)
        *(.embed.tab)
    }
}
