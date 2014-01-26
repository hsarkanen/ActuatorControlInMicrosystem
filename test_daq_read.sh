#!/bin/sh

# A traditional shell does not produce the same output
/usr/xenomai/bin/insn_read -v -d analogy0 -s 0 -S 3 -c 0 -R 0
sleep 10
/usr/xenomai/bin/insn_read -v -d analogy0 -s 0 -S 3 -c 0 -R 0
sleep 10
/usr/xenomai/bin/insn_read -v -d analogy0 -s 0 -S 3 -c 0 -R 0
sleep 10
/usr/xenomai/bin/insn_read -v -d analogy0 -s 0 -S 3 -c 0 -R 0
echo "Done"
