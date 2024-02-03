#ifndef _RETURN_TEMPLATE_H_
#define _RETURN_TEMPLATE_H_

// numeric literal
#define return_template_num \
"mov rax, 0x%x\n\
pop rbp\n\
ret\n\n"

// 1-byte
#define return_template_var1 \
"mov eax, byte [rbp-0x%x]\n\
pop rbp\n\
ret\n\n"

// 2-byte
#define return_template_var2 \
"mov eax, word [rbp-0x%x]\n\
pop rbp\n\
ret\n\n"

// 4-byte
#define return_template_var4 \
"mov eax, dword [rbp-0x%x]\n\
pop rbp\n\
ret\n\n"

// 8-byte
#define return_template_var8 \
"mov rax, qword [rbp-0x%x]\n\
pop rbp\n\
ret\n\n"

#endif
