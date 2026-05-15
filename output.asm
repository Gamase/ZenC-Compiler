section .data
    fmt_int         db "%d", 10, 0
    fmt_float       db "%.6g", 10, 0
    fmt_scanf_float db "%lf", 0
    fmt_scanf_int   db "%d", 0
    fmt_str         db "%s", 10, 0
    input_buffer    dq 0
    str0 db "al menos uno es verdadero", 0
    str1 db "todos verdaderos", 0
    str2 db "esto no debe imprimir", 0
    str3 db "correctamente falso", 0

section .bss
    x resq 1
    y resq 1
    z resq 1

section .text
    extern printf, scanf, strcmp, fmod
    global main

main:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    jmp __inicio


__inicio:
    mov rax, 5
    mov [rel x], rax
    mov rax, 10
    mov [rel y], rax
    mov rax, 3
    mov [rel z], rax
    mov rax, [rel x]
    cmp rax, 10
    setg al
    movzx rax, al
    mov [rbp - 8], rax
    mov rax, [rel y]
    cmp rax, 5
    setg al
    movzx rax, al
    mov [rbp - 16], rax
    mov rax, [rbp - 8]
    cmp rax, 0
    setne al
    movzx rax, al
    mov rbx, [rbp - 16]
    cmp rbx, 0
    setne bl
    movzx rbx, bl
    or rax, rbx
    mov [rbp - 24], rax
    mov rax, [rbp - 24]
    cmp rax, 0
    je L1
    sub rsp, 32
    lea rcx, [rel fmt_str]
    lea rdx, [rel str0]
    call printf
    add rsp, 32
    jmp L2
L1:
L2:
    mov rax, [rel x]
    cmp rax, 3
    setg al
    movzx rax, al
    mov [rbp - 32], rax
    mov rax, [rel y]
    cmp rax, 5
    setg al
    movzx rax, al
    mov [rbp - 40], rax
    mov rax, [rbp - 32]
    cmp rax, 0
    setne al
    movzx rax, al
    mov rbx, [rbp - 40]
    cmp rbx, 0
    setne bl
    movzx rbx, bl
    and rax, rbx
    mov [rbp - 48], rax
    mov rax, [rel z]
    cmp rax, 10
    setl al
    movzx rax, al
    mov [rbp - 56], rax
    mov rax, [rbp - 48]
    cmp rax, 0
    setne al
    movzx rax, al
    mov rbx, [rbp - 56]
    cmp rbx, 0
    setne bl
    movzx rbx, bl
    and rax, rbx
    mov [rbp - 64], rax
    mov rax, [rbp - 64]
    cmp rax, 0
    je L3
    sub rsp, 32
    lea rcx, [rel fmt_str]
    lea rdx, [rel str1]
    call printf
    add rsp, 32
    jmp L4
L3:
L4:
    mov rax, [rel x]
    cmp rax, 10
    setg al
    movzx rax, al
    mov [rbp - 72], rax
    mov rax, [rel y]
    cmp rax, 5
    setg al
    movzx rax, al
    mov [rbp - 80], rax
    mov rax, [rbp - 72]
    cmp rax, 0
    setne al
    movzx rax, al
    mov rbx, [rbp - 80]
    cmp rbx, 0
    setne bl
    movzx rbx, bl
    and rax, rbx
    mov [rbp - 88], rax
    mov rax, [rbp - 88]
    cmp rax, 0
    je L5
    sub rsp, 32
    lea rcx, [rel fmt_str]
    lea rdx, [rel str2]
    call printf
    add rsp, 32
    jmp L6
L5:
    sub rsp, 32
    lea rcx, [rel fmt_str]
    lea rdx, [rel str3]
    call printf
    add rsp, 32
L6:

    mov rax, 0
    leave
    ret
