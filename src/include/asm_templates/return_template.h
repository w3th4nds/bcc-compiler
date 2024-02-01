#ifndef _RETURN_TEMPLATE_H_
#define _RETURN_TEMPLATE_H_

#define return_template_asm \
"mov rax, %d\n\
pop rbp\n\
ret\n\n\
"

#define return_template_sz 36

#endif
