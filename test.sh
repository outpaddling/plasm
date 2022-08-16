#!/bin/sh -e

##########################################################################
#   Function description:
#       Pause until user presses return
##########################################################################

pause()
{
    local junk
    
    printf "Press return to continue..."
    read junk
}

export PATH=.:$PATH

make

./plasm test.ecisc
pause
more test.bin

./plasm test.riscv
pause
more test.bin
