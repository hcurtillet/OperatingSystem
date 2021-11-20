section .multiboot_header

magic equ 0xE85250D6                ; multiboot2
arch equ 0                          ; protected mode i386

header_start:
    dd magic                        ; magic number 
    dd arch                         ; architecture
    dd header_end - header_start    ; header lenght 

    dd 0x100000000 - (magic + arch + (header_end - header_start))

    dw 0                            ; type
    dw 0                            ; flag
    dd 8                            ; size
header_end: