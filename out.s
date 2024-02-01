global main

section .text

main:
push rbp
mov rbp, rsp
mov dword [rbp-0x4], 0x5
mov dword [rbp-0x8], 0x9
mov rax, 0x3
pop rbp
ret
