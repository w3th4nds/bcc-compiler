#!/bin/bash
echo "[*] Building asm into executable..."
nasm -g -F dwarf -f elf64 -o out.o out.s && gcc -o out out.o
echo "[*] Running it and checking the return value"
./out 
echo $?
