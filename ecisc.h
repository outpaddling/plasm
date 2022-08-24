/*
 *  This file should be included in statement.h.
 */

#ifndef _ECISC_H_
#define _ECISC_H_

#include <vector>
#include "statement.h"
#include "ecisc-bits.h"

class statement_ecisc : public statement
{
    public:
	statement_ecisc(void);
	int     translateInstruction(string::size_type endLabel);
	int     translateOpcode(void);
	int     translateOperand(string &operand, uint64_t *bits);
	bool    isComment(string::size_type start_pos);
	void    outputMl(ostream &outfile);
	inline int  get_dataSize(void) { return dataSize; }
	void    initTable(void);
    private:
	// FIXME: Can some of this just be local variables?
	int             dataSize;
	uint8_t         binaryOpcode;
	uint8_t         modeByte[MAX_OPERANDS];
	uint32_t        operandValue[MAX_OPERANDS];
	string          label[MAX_OPERANDS];
	vector<opcode>  opcodeTable;
};

#endif

