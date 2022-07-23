#include "epc.h"

void statement_epc :: initTable(void)
{
    opcodeTable.push_back(opcode("jl", OP_JL));
    opcodeTable.push_back(opcode("ret", OP_RET));
    opcodeTable.push_back(opcode("beq", OP_BEQ));
    opcodeTable.push_back(opcode("bne", OP_BNE));
    opcodeTable.push_back(opcode("blt", OP_BLT));
    opcodeTable.push_back(opcode("ble", OP_BLE));
    opcodeTable.push_back(opcode("bltu", OP_BLTU));
    opcodeTable.push_back(opcode("bleu", OP_BLEU));
    opcodeTable.push_back(opcode("bgt", OP_BGT));
    opcodeTable.push_back(opcode("bge", OP_BGE));
    opcodeTable.push_back(opcode("bgtu", OP_BGTU));
    opcodeTable.push_back(opcode("bgeu", OP_BGEU));
    opcodeTable.push_back(opcode("boc", OP_BOC));
    opcodeTable.push_back(opcode("bnc", OP_BNC));
    opcodeTable.push_back(opcode("bov", OP_BOV));
    opcodeTable.push_back(opcode("bnv", OP_BNV));
    opcodeTable.push_back(opcode("j", OP_J));
    opcodeTable.push_back(opcode("movb", OP_MOVB));
    opcodeTable.push_back(opcode("movs", OP_MOVS));
    opcodeTable.push_back(opcode("movl", OP_MOVL));
    opcodeTable.push_back(opcode("movq", OP_MOVQ));
    opcodeTable.push_back(opcode("movf", OP_MOVF));
    opcodeTable.push_back(opcode("movd", OP_MOVD));
    opcodeTable.push_back(opcode("movlf", OP_MOVLF));
    opcodeTable.push_back(opcode("movfl", OP_MOVFL));
    opcodeTable.push_back(opcode("movfd", OP_MOVFD));
    opcodeTable.push_back(opcode("movdf", OP_MOVDF));
    opcodeTable.push_back(opcode("cmpb", OP_CMPB));
    opcodeTable.push_back(opcode("cmps", OP_CMPS));
    opcodeTable.push_back(opcode("cmpl", OP_CMPL));
    opcodeTable.push_back(opcode("cmpq", OP_CMPQ));
    opcodeTable.push_back(opcode("cmpf", OP_CMPF));
    opcodeTable.push_back(opcode("cmpd", OP_CMPD));
    opcodeTable.push_back(opcode("notb", OP_NOTB));
    opcodeTable.push_back(opcode("nots", OP_NOTS));
    opcodeTable.push_back(opcode("notl", OP_NOTL));
    opcodeTable.push_back(opcode("notq", OP_NOTQ));
    opcodeTable.push_back(opcode("andb", OP_ANDB));
    opcodeTable.push_back(opcode("ands", OP_ANDS));
    opcodeTable.push_back(opcode("andl", OP_ANDL));
    opcodeTable.push_back(opcode("andq", OP_ANDQ));
    opcodeTable.push_back(opcode("orb", OP_ORB));
    opcodeTable.push_back(opcode("ors", OP_ORS));
    opcodeTable.push_back(opcode("orl", OP_ORL));
    opcodeTable.push_back(opcode("orq", OP_ORQ));
    opcodeTable.push_back(opcode("xorb", OP_XORB));
    opcodeTable.push_back(opcode("xors", OP_XORS));
    opcodeTable.push_back(opcode("xorl", OP_XORL));
    opcodeTable.push_back(opcode("xorq", OP_XORQ));
    opcodeTable.push_back(opcode("sllb", OP_SLLB));
    opcodeTable.push_back(opcode("slls", OP_SLLS));
    opcodeTable.push_back(opcode("slll", OP_SLLL));
    opcodeTable.push_back(opcode("sllq", OP_SLLQ));
    opcodeTable.push_back(opcode("srlb", OP_SRLB));
    opcodeTable.push_back(opcode("srls", OP_SRLS));
    opcodeTable.push_back(opcode("srll", OP_SRLL));
    opcodeTable.push_back(opcode("srlq", OP_SRLQ));
    opcodeTable.push_back(opcode("srab", OP_SRAB));
    opcodeTable.push_back(opcode("sras", OP_SRAS));
    opcodeTable.push_back(opcode("sral", OP_SRAL));
    opcodeTable.push_back(opcode("sraq", OP_SRAQ));
    opcodeTable.push_back(opcode("rolb", OP_ROLB));
    opcodeTable.push_back(opcode("rols", OP_ROLS));
    opcodeTable.push_back(opcode("roll", OP_ROLL));
    opcodeTable.push_back(opcode("rolq", OP_ROLQ));
    opcodeTable.push_back(opcode("clrb", OP_CLRB));
    opcodeTable.push_back(opcode("clrs", OP_CLRS));
    opcodeTable.push_back(opcode("clrl", OP_CLRL));
    opcodeTable.push_back(opcode("addb", OP_ADDB));
    opcodeTable.push_back(opcode("adds", OP_ADDS));
    opcodeTable.push_back(opcode("addl", OP_ADDL));
    opcodeTable.push_back(opcode("addq", OP_ADDQ));
    opcodeTable.push_back(opcode("addf", OP_ADDF));
    opcodeTable.push_back(opcode("addd", OP_ADDD));
    opcodeTable.push_back(opcode("addbc", OP_ADDBC));
    opcodeTable.push_back(opcode("addsc", OP_ADDSC));
    opcodeTable.push_back(opcode("addlc", OP_ADDLC));
    opcodeTable.push_back(opcode("incb", OP_INCB));
    opcodeTable.push_back(opcode("incs", OP_INCS));
    opcodeTable.push_back(opcode("incl", OP_INCL));
    opcodeTable.push_back(opcode("decb", OP_DECB));
    opcodeTable.push_back(opcode("decs", OP_DECS));
    opcodeTable.push_back(opcode("decl", OP_DECL));
    opcodeTable.push_back(opcode("subb", OP_SUBB));
    opcodeTable.push_back(opcode("subs", OP_SUBS));
    opcodeTable.push_back(opcode("subl", OP_SUBL));
    opcodeTable.push_back(opcode("subq", OP_SUBQ));
    opcodeTable.push_back(opcode("subf", OP_SUBF));
    opcodeTable.push_back(opcode("subd", OP_SUBD));
    opcodeTable.push_back(opcode("mulb", OP_MULB));
    opcodeTable.push_back(opcode("muls", OP_MULS));
    opcodeTable.push_back(opcode("mull", OP_MULL));
    opcodeTable.push_back(opcode("mulq", OP_MULQ));
    opcodeTable.push_back(opcode("mulf", OP_MULF));
    opcodeTable.push_back(opcode("muld", OP_MULD));
    opcodeTable.push_back(opcode("divb", OP_DIVB));
    opcodeTable.push_back(opcode("divs", OP_DIVS));
    opcodeTable.push_back(opcode("divl", OP_DIVL));
    opcodeTable.push_back(opcode("divq", OP_DIVQ));
    opcodeTable.push_back(opcode("divf", OP_DIVF));
    opcodeTable.push_back(opcode("divd", OP_DIVD));
    opcodeTable.push_back(opcode("remb", OP_REMB));
    opcodeTable.push_back(opcode("rems", OP_REMS));
    opcodeTable.push_back(opcode("reml", OP_REML));
    opcodeTable.push_back(opcode("remq", OP_REMQ));
    opcodeTable.push_back(opcode("inputb", OP_INPUTB));
    opcodeTable.push_back(opcode("inputs", OP_INPUTS));
    opcodeTable.push_back(opcode("inputl", OP_INPUTL));
    opcodeTable.push_back(opcode("outputb", OP_OUTPUTB));
    opcodeTable.push_back(opcode("outputs", OP_OUTPUTS));
    opcodeTable.push_back(opcode("outputl", OP_OUTPUTL));
    opcodeTable.push_back(opcode("sleepl", OP_SLEEPL));
    opcodeTable.push_back(opcode("halt", OP_HALT));
}
