#!/bin/bash
echo "#########################"
echo "[*] Testing simple.c"
SHOW_SOURCE=0 ./bcc examples/simple.c

echo "#########################"
echo "[*] Testing simple2.c"
SHOW_SOURCE=0 ./bcc examples/simple2.c

echo "#########################"
echo "[*] Testing simple3.c"
SHOW_SOURCE=0./bcc examples/simple3.c

echo "#########################"
echo "[*] Testing simple4.c"
SHOW_SOURCE=0./bcc examples/simple4.c

echo "#########################"
echo "[*] Testing simple5.c"
SHOW_SOURCE=0 ./bcc examples/simple5.c


# maybe someday
#./bcc examples/test.c
