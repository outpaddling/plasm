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
    opcodeTable.push_back(opcode("lb", RISCV_OP_LB));
    opcodeTable.push_back(opcode("lh", RISCV_OP_LH));
    opcodeTable.push_back(opcode("lw", RISCV_OP_LW));
    opcodeTable.push_back(opcode("ld", RISCV_OP_LD));
    opcodeTable.push_back(opcode("lbu", RISCV_OP_LBU));
    opcodeTable.push_back(opcode("lhu", RISCV_OP_LHU));
    opcodeTable.push_back(opcode("lwu", RISCV_OP_LWU));
    opcodeTable.push_back(opcode("fence", RISCV_OP_FENCE));
    opcodeTable.push_back(opcode("fencei", RISCV_OP_FENCEI));
    opcodeTable.push_back(opcode("addi", RISCV_OP_ADDI));
    opcodeTable.push_back(opcode("slli", RISCV_OP_SLLI));
    opcodeTable.push_back(opcode("slti", RISCV_OP_SLTI));
    opcodeTable.push_back(opcode("sltiu", RISCV_OP_SLTIU));
    opcodeTable.push_back(opcode("xori", RISCV_OP_XORI));
    opcodeTable.push_back(opcode("srli", RISCV_OP_SRLI));
    opcodeTable.push_back(opcode("srai", RISCV_OP_SRAI));
    opcodeTable.push_back(opcode("ori", RISCV_OP_ORI));
    opcodeTable.push_back(opcode("andi", RISCV_OP_ANDI));
    opcodeTable.push_back(opcode("jalr", RISCV_OP_JALR));
    opcodeTable.push_back(opcode("lui", RISCV_OP_LUI));
    opcodeTable.push_back(opcode("jal", RISCV_OP_JAL));
    opcodeTable.push_back(opcode("sb", RISCV_OP_SB));
    opcodeTable.push_back(opcode("sh", RISCV_OP_SH));
    opcodeTable.push_back(opcode("sw", RISCV_OP_SW));
    opcodeTable.push_back(opcode("sd", RISCV_OP_SD));
    opcodeTable.push_back(opcode("beq", RISCV_OP_BEQ));
    opcodeTable.push_back(opcode("bne", RISCV_OP_BNE));
    opcodeTable.push_back(opcode("blt", RISCV_OP_BLT));
    opcodeTable.push_back(opcode("bge", RISCV_OP_BGE));
    opcodeTable.push_back(opcode("bltu", RISCV_OP_BLTU));
    opcodeTable.push_back(opcode("bgeu", RISCV_OP_BGEU));
}
