# BCC - Bad C Compiler

**Building a basic C compiler from scratch, for educational purposes and bragging rights.**

Outputs `x86 64-bit assembly`, nasm & gcc are used from there to create the binary file.

The goal is to compile basic programs, `#includes`, `typedefs` and other fancy things are forbidden.

This is a **non-optimizing compiler**, it doesn't make use of an intermediate representation phase.

Simplicity is valued more highly than support for more features. Will try to keep the entire thing under 3000 lines.

Taking inspiration from other (hobby) compilers, **no** memory management **is** the memory management scheme here.

Will not compile itself. Unless things get out of hand.

**Work in progress**

An example of what `bcc` can do as of now:

```
$  DEBUG=SCOPE,ASM ./bcc tests/test11.c && ./build.sh 

DEBUGGING:
    LEX = 0
  PARSE = 0
  SCOPE = 1
    ASM = 1

Source Code:
// should return 50
int main(void)
{
  int y = 5 * 3 * ((4 + 3) * 10) - 1040;
  long x = 0x10;
  int a = x + y * 2;
  return a + 7 * 2;
}

Setting scope -> global
Setting scope -> "main"
Current_scope: main - adding: "y" TYPE_INT
Current_scope: main - adding: "x" TYPE_LONG
Current_scope: main - adding: "a" TYPE_INT

Global scope:
Symtab:
---

main:
Return type: TYPE_INT
Params: (TYPE_VOID (null))
Symtab:

[ y               | TYPE_INT     | sz: 4 | off: 0x4  ]
------------------------------------------------------
[ x               | TYPE_LONG    | sz: 8 | off: 0x10 ]
------------------------------------------------------
[ a               | TYPE_INT     | sz: 4 | off: 0x14 ]
---
asm_generate()
asm_func_def()
Setting scope -> "main"
asm_assignment()
binop_evaluate()
asm_assignment()
asm_assignment()
binop_evaluate()
asm_return()
binop_evaluate()

Generated ASM =
global main

section .text

main:
push rbp
mov rbp, rsp
mov dword [rbp-0x4], 0xa
mov qword [rbp-0x10], 0x10
mov rbx, qword [rbp-0x10]
mov r8d, dword [rbp-0x4]
mov r9, 0x2
imul r8, r9
add rbx, r8
mov dword [rbp-0x14], ebx
mov ebx, dword [rbp-0x14]
mov r8, 0x7
mov r9, 0x2
imul r8, r9
add rbx, r8
mov eax, ebx
pop rbp
ret

[ SUCCESS ]
[*] Building asm into executable...
[*] Running it and checking the return value
50
```

**Notes:**

```
Q: Why is there no "sub rsp, X" in all my functions, even though local variables are declared ?

A: The System V ABI for x86-64 specifies a red zone of 128 bytes below %rsp. These 128 bytes belong to the function as long as it doesn't call any other function (it is a leaf function).

Signal handlers (and functions called by a debugger) need to respect the red zone, since they are effectively involuntary function calls.
All of the local variables of your test_function, which is a leaf function, fit into the red zone, thus no adjustment of %rsp is needed. (Also, the function has no visible side-effects and would be optimized out on any reasonable optimization setting).

You can compile with -mno-red-zone to stop the compiler from using space below the stack pointer. Kernel code has to do this because hardware interrupts don't implement a red-zone.

--

TODO:

********** implement leaf function detection in asm phase
******* get puts working to print outside the range of [0, 255]
**** clean up lexer - repeating code
**** add unary operator support

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
