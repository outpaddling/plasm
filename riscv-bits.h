
/*
 *  Define opcodes as 32-bit values including opcode and funct*
 *  The assembler then only needs to insert the operands.
 *  Rightmost 7 bits (opcode) determine instruction format.
 *  Additional funct* fields distinguish instructions.
 *
 *  R-type: funct7 rs2 rs1 funct3 rd opcode
 */

#define RISCV_OP_ADD    0x00000033  // 0000000 ----- ----- 000 ----- 0110011
#define RISCV_OP_SUB    0x40000033  // 0100000 ----- ----- 000 ----- 0110011
#define RISCV_OP_SLL    0x00001033  // 0000000 ----- ----- 001 ----- 0110011
#define RISCV_OP_SLT    0x00002033  // 0000000 ----- ----- 010 ----- 0110011
#define RISCV_OP_SLTU   0x00003033  // 0000000 ----- ----- 011 ----- 0110011
#define RISCV_OP_XOR    0x00004033  // 0000000 ----- ----- 100 ----- 0110011
#define RISCV_OP_SRL    0x00005033  // 0000000 ----- ----- 101 ----- 0110011
#define RISCV_OP_SRA    0x40005033  // 0100000 ----- ----- 101 ----- 0110011
#define RISCV_OP_OR     0x00006033  // 0000000 ----- ----- 110 ----- 0110011
#define RISCV_OP_AND    0x00007033  // 0000000 ----- ----- 111 ----- 0110011

/*
 *  I-type: immediate(12) rs1 funct3 rd opcode
 */

#define RISCV_OP_LB     0x00000003  // ------------  ----- 000 ----- 0000011
#define RISCV_OP_LH     0x00001003  // ------------  ----- 001 ----- 0000011
#define RISCV_OP_LW     0x00002003  // ------------  ----- 010 ----- 0000011
#define RISCV_OP_LD     0x00003003  // ------------  ----- 011 ----- 0000011
#define RISCV_OP_LBU    0x00004003  // ------------  ----- 100 ----- 0000011
#define RISCV_OP_LHU    0x00005003  // ------------  ----- 101 ----- 0000011
#define RISCV_OP_LWU    0x00006003  // ------------  ----- 110 ----- 0000011

#define RISCV_OP_FENCE  0x0000600F  // ------------  ----- 000 ----- 0001111
#define RISCV_OP_FENCEI 0x0000100F  // ------------  ----- 001 ----- 0001111

#define RISCV_OP_ADDI   0x00000013  // ------------  ----- 000 ----- 0010011
#define RISCV_OP_SLLI   0x00001013  // 0000000-----  ----- 001 ----- 0010011
#define RISCV_OP_SLTI   0x00002013  // ------------  ----- 010 ----- 0010011
#define RISCV_OP_SLTIU  0x00003013  // ------------  ----- 011 ----- 0010011
#define RISCV_OP_XORI   0x00004013  // ------------  ----- 100 ----- 0010011
#define RISCV_OP_SRLI   0x00005013  // 0000000-----  ----- 101 ----- 0010011
#define RISCV_OP_SRAI   0x40005013  // 0100000-----  ----- 101 ----- 0010011
#define RISCV_OP_ORI    0x00006013  // ------------  ----- 110 ----- 0010011
#define RISCV_OP_ANDI   0x00007013  // ------------  ----- 111 ----- 0010011

#define RISCV_OP_JALR   0x00000067  // ------------  ----- 000 ----- 1100111

/*
 *  U offset(20) rd opcode
 */

#define RISCV_OP_LUI    0x00000037  // -------------------- ----- 0110111

/*
 *  UJ offset(20) rd opcode
 */

#define RISCV_OP_JAL    0x0000006F  // -------------------- ----- 1101111

/*
 *  S-type: offset rs1 rs2 funct3 offset5 opcode
 */

#define RISCV_OP_SB     0x00000023  // ------- ----- ----- 000 00000 0100011
#define RISCV_OP_SH     0x00001023  // ------- ----- ----- 001 00000 0100011
#define RISCV_OP_SW     0x00002023  // ------- ----- ----- 010 00000 0100011
#define RISCV_OP_SD     0x00003023  // ------- ----- ----- 011 00000 0100011

/*
 *  SB-type: offset rs1 rs2 funct3 offset5 opcode
 */

#define RISCV_OP_BEQ    0x00000063  // ------------  ----- 000 ----- 1100011
#define RISCV_OP_BNE    0x00001063  // ------------  ----- 001 ----- 1100011
#define RISCV_OP_BLT    0x00004063  // ------------  ----- 100 ----- 1100011
#define RISCV_OP_BGE    0x00005063  // ------------  ----- 101 ----- 1100011
#define RISCV_OP_BLTU   0x00006063  // ------------  ----- 110 ----- 1100011
#define RISCV_OP_BGEU   0x00007063  // ------------  ----- 111 ----- 1100011

// Not bits in a mode by like ECISC
#define RISCV_MODE_UNKNOWN      0
#define RISCV_MODE_REG_DIRECT   1
#define RISCV_MODE_IMMEDIATE    2
#define RISCV_MODE_OFFSET       3
