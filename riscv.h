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
	void    translateOpcode(void);
	void    translateOperand(string &operand);
	bool    isComment(string::size_type start_pos);
	void    outputMl(ostream &outfile);
	void    initTable(void);
    private:
	//uint32_t        binaryOpcode;
	string          label[MAX_OPERANDS];
	uint32_t        binaryOpcode;
	uint32_t        operandValue[MAX_OPERANDS]; // FIXME: Type??
	uint32_t        machineInstruction;
	vector<opcode>  opcodeTable;
};

#endif

