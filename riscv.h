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
	int     translateInstruction(string::size_type endLabel);
	int     translateRtype(string::size_type endLabel);
	int     translateItype(string::size_type endLabel);
	int     translateLoad(string::size_type endLabel);
	int     translateStype(string::size_type endLabel);
	int     translateOpcode(void);
	int     translateOperand(string &operand, uint64_t *bits);
	bool    isComment(string::size_type start_pos);
	void    outputMl(ostream &outfile);
	void    initTable(void);
    private:
	uint32_t        machineInstruction;
	vector<opcode>  opcodeTable;
};

#endif

