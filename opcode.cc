#include "opcode.h"

opcode :: opcode(void)

{
    assem = "";
    bin = 0;
}


opcode :: opcode(const char *new_assem, uint32_t new_bin)

{
    assem = new_assem;
    bin = new_bin;
}


opcode :: opcode(const string &new_assem, uint32_t new_bin)

{
    assem = new_assem;
    bin = new_bin;
}


bool opcode_compare(const opcode &op1, const opcode &op2)

{
    return ((opcode)op1).get_assem() < ((opcode)op2).get_assem();
}

