#include "riscv.h"

void statement_riscv :: initTable(void)
{
    opcodeTable.push_back(opcode("add", RISCV_OP_ADD));
    opcodeTable.push_back(opcode("sub", RISCV_OP_SUB));
    opcodeTable.push_back(opcode("addi", RISCV_OP_ADDI));
    opcodeTable.push_back(opcode("ld", RISCV_OP_LD));
    opcodeTable.push_back(opcode("sd", RISCV_OP_SD));
}
