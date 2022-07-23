#include "riscv.h"

void statement_riscv :: initTable(void)
{
    opcodeTable.push_back(opcode("add", OP_ADD));
    opcodeTable.push_back(opcode("sub", OP_SUB));
    opcodeTable.push_back(opcode("addi", OP_ADDI));
    opcodeTable.push_back(opcode("ld", OP_LD));
    opcodeTable.push_back(opcode("sd", OP_SD));
}
