# BCC - Bad C Compiler

Building a basic C compiler, for educational purposes and bragging rights.

The goal is to compile basic programs, "#include"'s, typedefs and other fancy things are forbidden.

Will not compile itself. Unless things get out of hand.

```
SHOW_SOURCE=0 DEBUG=ASM ./bcc examples/simple.c && ./build.sh
```

Work in progress

```
TODO:

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
