/*
 *  This file should be included in statement.h.
 */

#ifndef _AS68K_H_
#define _AS68K_H_

class statement68k : public statement
{
    public:
	int     translateInstruction(string::size_type endLabel);
	int     translateOpcode(void);
	int     translateOperand(string &operand, uint64_t *bits);
	bool    isComment(string::size_type start_pos);
	void    outputMl(ostream &outfile);
    private:
	unsigned short  instruction;    // Bit codes
};

#endif

