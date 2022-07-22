/*
 *  This file should be included in statement.h.
 */

#ifndef _EPC_H_
#define _EPC_H_

#include <vector>
#include "epc-bits.h"

#define MACHINE_CODE_FIELD_WIDTH    40

class statement_epc : public statement
{
    public:
	statement_epc(void);
	void    translateOpcode(void);
	void    translateOperand(string &operand);
	bool    isComment(string::size_type start_pos);
	void    outputMl(ostream &outfile);
	inline int  get_dataSize(void) { return dataSize; }
    private:
	int             dataSize;
	uint8_t         binaryOpcode;
	uint8_t         modeByte[MAX_OPERANDS];
	uint32_t        operandValue[MAX_OPERANDS];
	string          label[MAX_OPERANDS];
	vector<opcode>  opcodeTable;
};

#endif

