global main

section .text

main:
push rbp
mov rbp, rsp
mov dword [rbp-4], 3
mov dword [rbp-8], 4
mov rax, 24
pop rbp
ret

