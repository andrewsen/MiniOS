global enable_paging:function enable_paging.end-enable_paging
global enable_fpu:function enable_fpu.end-enable_fpu
global get_capacity:function get_capacity.end-get_capacity

global rand:function rand.end-rand
global sin:function sin.end-sin
global cos:function cos.end-cos
global fabs:function fabs.end-fabs
global sqrt:function sqrt.end-sqrt

enable_paging:
    mov     eax, cr0
    or      eax, 80000000h
    mov     cr0, eax
    ret
.end:

enable_fpu:
    mov     eax, cr0
    and     eax, 0FFFFFFF3h ; clear cr0.TS and cr0.EM to fully enable fpu
    mov     cr0, eax
    fnclex
    fninit

    fnstsw [fputest]                      ; store status word
    cmp word [fputest], 0                 ; compare the written status with the expected FPU state
    jne nofpu                             ; jump if the FPU hasn't written anything
    mov     eax, 1
    ret
nofpu:
    xor     eax, eax
    ret

    fputest: dw 055AAh
enable_fpu.end:

get_capacity:
    push    ebp
    mov     ebp, esp

    fld1
    fild    dword [ebp+8]
    fyl2x
    fisttp  dword [ebp+8]

    mov     eax, 1
    mov     ecx, [ebp+8]
    inc     ecx
    shl     eax, cl

    mov     esp, ebp
    pop     ebp
    ret
get_capacity.end:

rand:
    ;mov     dr0, eax
    ;mov     dr1, ebx
    ;mov     dr2, ecx
    ;mov     dr3, edx
    rdrand  eax
    ret
rand.end:

sin:
    push    ebp
    mov     ebp, esp

    fld     qword [ebp+8]
    fsin

    mov     esp, ebp
    pop     ebp
    ret
sin.end:

cos:
    push    ebp
    mov     ebp, esp

    fld     qword [ebp+8]
    fcos

    mov     esp, ebp
    pop     ebp
    ret
cos.end:

sqrt:
    push    ebp
    mov     ebp, esp

    fld     qword [ebp+8]
    fsqrt

    mov     esp, ebp
    pop     ebp
    ret
sqrt.end:

fabs:
    push    ebp
    mov     ebp, esp

    fld     qword [ebp+8]
    fabs

    mov     esp, ebp
    pop     ebp
    ret
fabs.end:
