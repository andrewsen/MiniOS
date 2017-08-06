MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table. GRUB will also require our kernel be in elf32 format.
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

bits 32                         ; All instructions should be 32-bit.
section .multiboot
align 4


dd  MBOOT_HEADER_MAGIC      ; GRUB will search for this value on each
			; 4-byte boundary in the first 8KB of your kernel file.
dd  MBOOT_HEADER_FLAGS      ; How GRUB should load your file / settings
dd  MBOOT_CHECKSUM          ; To ensure that the above values are correct



section .text

global loader:function loader.end-loader ; Kernel entry
extern main			                  ; C entry point
extern reboot
;extern end


loader:
    ;cli
    mov esp, stack
    push ebx                  	; multiboot info structure ptr
    mov ebp, 0

    call main
        call reboot
hlt_loop:
	hlt
    jmp hlt_loop
loader.end:


section .bss
	align 16
    resb 65536
stack:
