/*
 *  This file should be included in statement.h.
 */

#ifndef _ECISC_H_
#define _ECISC_H_

#include <vector>
#include "statement.h"
#include "ecisc-bits.h"

#define MACHINE_CODE_FIELD_WIDTH    40

class statement_ecisc : public statement
{
    public:
	statement_ecisc(void);
	void    translateInstruction(string::size_type endLabel);
	void    translateOpcode(void);
	void    translateOperand(string &operand);
	bool    isComment(string::size_type start_pos);
	void    outputMl(ostream &outfile);
	inline int  get_dataSize(void) { return dataSize; }
	void    initTable(void);
    private:
	int             dataSize;
	uint8_t         binaryOpcode;
	uint8_t         modeByte[MAX_OPERANDS];
	uint32_t        operandValue[MAX_OPERANDS];
	string          label[MAX_OPERANDS];
	vector<opcode>  opcodeTable;
};

#endif

