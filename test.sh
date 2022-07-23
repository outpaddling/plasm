#!/bin/sh -e

export PATH=.:$PATH

make

./plasm test.epc
more test.bin

./plasm test.riscv
more test.bin
