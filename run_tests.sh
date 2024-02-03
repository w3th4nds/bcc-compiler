#!/bin/bash
for i in {1..8}
do
  echo "###################"
  echo "[*] Testing test"$i".c"
  SHOW_SOURCE=0 ./bcc tests/test"$i".c
done
