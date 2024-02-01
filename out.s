global main

section .text

main:
push rbp
mov rbp, rsp
mov dword [rbp-4], 5
mov dword [rbp-8], 9
mov rax, 3
pop rbp
ret

