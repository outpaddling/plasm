#include "riscv.h"

void statement_riscv :: initTable(void)
{
    opcodeTable.push_back(opcode("add", RISCV_OP_ADD));
    opcodeTable.push_back(opcode("sub", RISCV_OP_SUB));
    opcodeTable.push_back(opcode("sll", RISCV_OP_SLL));
    opcodeTable.push_back(opcode("slt", RISCV_OP_SLT));
    opcodeTable.push_back(opcode("sltu", RISCV_OP_SLTU));
    opcodeTable.push_back(opcode("xor", RISCV_OP_XOR));
    opcodeTable.push_back(opcode("srl", RISCV_OP_SRL));
    opcodeTable.push_back(opcode("sra", RISCV_OP_SRA));
    opcodeTable.push_back(opcode("or", RISCV_OP_OR));
    opcodeTable.push_back(opcode("and", RISCV_OP_AND));
    opcodeTable.push_back(opcode("addi", RISCV_OP_ADDI));
    opcodeTable.push_back(opcode("ld", RISCV_OP_LD));
    opcodeTable.push_back(opcode("sd", RISCV_OP_SD));
}
