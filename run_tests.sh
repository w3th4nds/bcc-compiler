#!/bin/bash

white="\e[97m"
green="\e[32m"
red="\e[31m"
bold="\e[1m"
echo -ne "$white$bold"

testdir=tests

# test if dir exists
if [ ! -d "$testdir" ]; then
  exit 1
fi

for file in `ls $testdir/*.c | sort -V`; do
  # compile & build, save return code
  SHOW_SOURCE=0 ./bcc $file
  nasm -g -F dwarf -f elf64 -o out.o out.s && gcc -o out out.o
  ./out
  ret_code=$?
  # compile with gcc, and compare
  gcc $file
  ./a.out
  gcc_ret_code=$?
  echo -en "[*] $file:\t["
  if [ "$ret_code" -ne "$gcc_ret_code" ]; then
    echo -en "$red FAIL "$white"] "
    echo -en "($red bcc: $ret_code$white | "
    echo -e "$green gcc: $gcc_ret_code$white )"
    #echo "Test source:"
    #cat "$file"
  else
    echo -e "$green PASS "$white"]"
  fi

done

# remove gcc binary
rm a.out
