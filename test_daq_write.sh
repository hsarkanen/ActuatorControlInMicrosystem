#!/bin/sh

# A traditional shell does not produce the same output
/usr/xenomai/bin/insn_write -v -d analogy0 -s 1 -c 0 -V 453619
sleep 10
/usr/xenomai/bin/insn_write -v -d analogy0 -s 1 -c 0 -V 0
sleep 10
/usr/xenomai/bin/insn_write -v -d analogy0 -s 1 -c 0 -V 453619
sleep 10
/usr/xenomai/bin/insn_write -v -d analogy0 -s 1 -c 0 -V 0
echo "Done"
