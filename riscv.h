/*
 *  This file should be included in statement.h.
 */

#ifndef _RISCV_H_
#define _RISCV_H_

#include <vector>
#include "riscv-bits.h"

class statement_riscv : public statement
{
    public:
	statement_riscv(void);
	void    translateOpcode(void);
	void    translateOperand(string &operand);
	bool    isComment(string::size_type start_pos);
	void    outputMl(ostream &outfile);
    private:
	//uint32_t        binaryOpcode;
	string          label;
	uint32_t        instruction;    // Bit codes
	vector<opcode>  opcodeTable;
};

#endif

