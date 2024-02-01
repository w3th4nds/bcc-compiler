# BCC - Bad C Compiler

Building a basic C compiler, for educational purposes and bragging rights.

The goal is to compile basic programs, "#include"'s, typedefs and other fancy things are forbidden.

Will not compile itself. Unless things get out of hand.

**Work in progress**

Example:

```
$ DEBUG=SCOPE,ASM ./bcc tests/test3.c && ./build.sh                                                                                       

DEBUGGING:
    LEX = 0
  PARSE = 0
  SCOPE = 1
    ASM = 1

Source Code:
// comment
int main(void)
{
  int a = 3;
  int b = 4;
  return 3 * 5 + 9;
}

Setting scope -> global
Setting scope -> "main"
Current_scope: main - adding: "a" TYPE_INT
Current_scope: main - adding: "b" TYPE_INT

Global scope:
Symtab:
---

main:
Return type: TYPE_INT
Params: (TYPE_VOID (null))
Symtab:

[ a               | TYPE_INT     | sz: 4 | off: 0x0  ]
------------------------------------------------------
[ b               | TYPE_INT     | sz: 4 | off: 0x4  ]
---
asm_generate()
asm_func_def()
Setting scope -> "main"
asm_assignment
asm_assignment
asm_return()

Generated ASM =
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

[ SUCCESS ]
[*] Building asm into executable...
[*] Running it and checking the return value
24
```


Notes:

```
Q: Why is there no "sub rsp, X" in all my functions, even though local variables are declared ?

A: The System V ABI for x86-64 specifies a red zone of 128 bytes below %rsp. These 128 bytes belong to the function as long as it doesn't call any other function (it is a leaf function).

Signal handlers (and functions called by a debugger) need to respect the red zone, since they are effectively involuntary function calls.
All of the local variables of your test_function, which is a leaf function, fit into the red zone, thus no adjustment of %rsp is needed. (Also, the function has no visible side-effects and would be optimized out on any reasonable optimization setting).

You can compile with -mno-red-zone to stop the compiler from using space below the stack pointer. Kernel code has to do this because hardware interrupts don't implement a red-zone.

--

TODO:

**** remove params from symtabs
**** add unary operator support

next up - assembly generation

registers
--
useful structure to use in the asm_gen phase

labels
--
generate labels for every point we will be jumping to in the source code

keep track of general purpose registers - their names, and if they are in use.
pick the first not in use register to use


Parsing
--
implement conditionals, loops etc

Lexing
--
eventually implement more keywords to detect, but get basic asm generation working first

```
