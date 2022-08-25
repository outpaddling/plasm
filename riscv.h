/*
 *  This file should be included in statement.h.
 */

#ifndef _RISCV_H_
#define _RISCV_H_

#include <vector>
#include "statement.h"
#include "riscv-bits.h"

class statement_riscv : public statement
{
    public:
	statement_riscv(void);
	int     translateInstruction(TranslationUnit *transUnit, string::size_type endLabel);
	int     translateRtype(TranslationUnit *transUnit, string::size_type endLabel);
	int     translateItype(TranslationUnit *transUnit, string::size_type endLabel);
	int     translateLoad(TranslationUnit *transUnit, string::size_type endLabel);
	int     translateStype(TranslationUnit *transUnit, string::size_type endLabel);
	int     translateOpcode(TranslationUnit *transUnit);
	int     translateOperand(TranslationUnit *transUnit, string &operand, uint64_t *bits);
	bool    isComment(string::size_type start_pos);
	void    outputMl(TranslationUnit *transUnit, ostream &outfile);
	void    initTable(void);
    private:
	uint32_t        machineInstruction;
	vector<opcode>  opcodeTable;
	string          labelOperand;
};

#endif

