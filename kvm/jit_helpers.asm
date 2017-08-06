global jitCallHelper
extern jitCheckSetPriority
extern jitIsCompiled

;.intel_syntax noprefix

;push [0]
;...
;push [n]
;push &jit_func

section .text
jitCallHelper:
    push ebp
    mov ebp, esp

    push esi
    push edi
    ;;;;;;;;

    push dword [ebp+8]            ; addr of jit_func
    call jitCheckSetPriority
    add esp, 4              ;restore stack after call

    cmp eax, 0
    je WAIT_LOOP

    ;;;;;;;;
    pop edi
    pop esi

    mov esp, ebp
    pop ebp
    ret

WAIT_LOOP:
    push dword [ebp+8]
    call jitIsCompiled
    add esp, 4
    cmp eax, 0
    je WAIT_LOOP

    ;;;;;;;;
    pop edi
    pop esi

    mov esp, ebp
    pop ebp

    ;mov dword eax, [eax]

    ret

;.att_syntax noprefix
