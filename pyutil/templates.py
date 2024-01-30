#!/usr/bin/env python3

FNAME = 'funcdef_template'
TEMPLATE = '''"\
%s:\\n\\
push rbp\\n\\
mov rbp, rsp\\n\\
sub rsp, %d\\n\
"'''

header = f'_{FNAME.upper()}_H_'
file_contents = f'''\
#ifndef {header}
#define {header}

#define {FNAME}_asm \\
{TEMPLATE}

#define {FNAME}_sz {len(TEMPLATE)}

#endif\
'''
print(file_contents)
with open(f'../src/include/asm_templates/{FNAME}.h', 'w') as f:
  f.write(file_contents)
