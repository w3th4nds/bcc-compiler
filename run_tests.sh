#!/bin/bash

white="\e[97m"
green="\e[32m"
red="\e[31m"
bold="\e[1m"
echo -ne "$white$bold"

testdir="tests/"

# test if dir exists
if [ ! -d "$testdir" ]; then
  exit 1
fi

for file in `ls $testdir/*.c | sort -V`; do
  # check if file exists
  if [ ! -f "$file" ]; then
    echo -e "$red ** Missing $file **"
    exit 1
  fi
  # compile & build, save return code
  SHOW_SOURCE=0 ./bcc $file
  nasm -g -F dwarf -f elf64 -o out.o out.s && gcc -o out out.o
  ./out
  ret_code=$?
  # compile with gcc, and compare
  gcc $file
  ./a.out
  gcc_ret_code=$?
  echo -en "Testing $file: ["
  if [ "$ret_code" -ne "$gcc_ret_code" ]; then
    echo -e "$red FAIL "$white"]"
    echo "** bcc: $ret_code"
    echo "** gcc: $gcc_ret_code"
    break
  else
    echo -e "$green PASS "$white"]"
  fi

done

# remove gcc binary
rm a.out
