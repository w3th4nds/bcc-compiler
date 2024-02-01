global main

section .text

main:
push rbp
mov rbp, rsp
mov dword [rbp-0x4], 0x3
mov qword [rbp-0x10], 0x5
mov dword [rbp-0x14], 0x4
mov rax, 0xcc
pop rbp
ret
