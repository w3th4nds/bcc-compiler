#ifndef _FUNCDEF_TEMPLATE_H_
#define _FUNCDEF_TEMPLATE_H_

#define funcdef_template_asm \
"%s:\n\
push rbp\n\
mov rbp, rsp\n\
sub rsp, %d\n"

#define funcdef_template_sz 50

#endif