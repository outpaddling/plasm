/*
 *  This file should be included in statement.h.
 */

#ifndef _RISCV_H_
#define _RISCV_H_

class statement_riscv : public statement
{
    public:
	void    translateOpcode(void);
	void    translateOperand(string &operand);
	bool    isComment(string::size_type start_pos);
	void    outputMl(ostream &outfile);
    private:
	uint32_t    instruction;    // Bit codes
};

#endif

