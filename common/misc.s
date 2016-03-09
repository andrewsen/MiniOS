
global enable_paging:function enable_paging.end-enable_paging ; Allows the C code to call cr0_flush().

enable_paging:
        mov cr0, eax
        or eax, 80000000h
        mov eax, cr0
    ret
.end:
