
/*
 *  Define opcodes as 32-bit values including opcode and funct*
 *  The assembler then only needs to insert the operands.
 *  Rightmost 7 bits (opcode) determine instruction format.
 *  Additional funct* fields distinguish instructions.
 *
 *  R-type: funct7 rs2 rs1 funct3 rd opcode
 *  I-type: immediate(12) rs1 funct3 rd opcode
 *  S-type: offset7 rs1 rs2 funct3 offset5 opcode
 */

#define OP_ADD  0x00000033  // 0000000 ----- ----- 000 ----- 0110011
#define OP_SUB  0x40000033  // 0100000 ----- ----- 000 ----- 0110011
#define OP_ADDI 0x00000013  // ------------  ----- 000 ----- 0010011
#define OP_LD   0x00003003  // ------------  ----- 011 ----- 0000011
#define OP_SD   0x00003023  // ------- ----- ----- 011 00000 0100011
