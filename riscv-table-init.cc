#include "riscv.h"

void statement_riscv :: initTable(void)
{
    opcodeTable.push_back(opcode("", OP_ADD));
    opcodeTable.push_back(opcode("", OP_SUB));
    opcodeTable.push_back(opcode("", OP_ADDI));
    opcodeTable.push_back(opcode("", OP_LD));
    opcodeTable.push_back(opcode("", OP_SD));
}
