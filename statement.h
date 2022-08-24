#ifndef _STATEMENT_H_
#define _STATEMENT_H_

#include <string>

#ifndef _SYMTABLE_H_
#include "symtable.h"
#endif

using namespace std;

#define MAX_OPERANDS                3

#define STATEMENT_OK                    0x00000000
#define STATEMENT_COMMENT               0x00000001
#define STATEMENT_MISSING_COLON         0x00000002
#define STATEMENT_BAD_LABEL             0x00000004
#define STATEMENT_INVALID_OPCODE        0x00000008
#define STATEMENT_INVALID_OPERAND       0x00000010
#define STATEMENT_MISSING_INITIALIZER   0x00000020
#define STATEMENT_OPERAND_COUNT         0x00000040
#define STATEMENT_INVALID_DIRECTIVE     0x00000080
#define STATEMENT_EXPECTED_REGISTER     0x00000100
#define STATEMENT_EXPECTED_OFFSET       0x00000200

#define TYPE_BYTE   1
#define TYPE_SHORT  2
#define TYPE_LONG   3
#define TYPE_QUAD   4
#define TYPE_FLOAT  5
#define TYPE_DOUBLE 6
#define TYPE_STRING 7

typedef unsigned long   statement_status_t;

class statement
{
    public:
	istream         &read(istream &infile);
	void            parse(mc_offset_t source_line);
	void            processDirective(string::size_type endLabel);
	void            processInitializers(string::size_type endLabel);
	void            printErrors(const char *filename,
				    unsigned long line);

	bool            hasLabel(void);
	bool            badLabel(void);

	// Architecture-dependent: Must be implemented by derived class
	virtual int     translateInstruction(string::size_type endLabel) = 0;
	virtual int     translateOpcode(void) = 0;
	virtual int     translateOperand(string &operand, uint64_t *bits) = 0;
	virtual bool    isComment(string::size_type start_post) = 0;
	virtual void    outputMl(ostream &outfile) = 0;
	//virtual void    initTable(void) = 0;
	
	// Mutators, accessors, etc.
	unsigned int    get_operandCount(void) { return operandCount; }
	string   &get_label(void) { return label; }
	void     set_label(const string &newLabel) { label = newLabel; }
	void     increment_operandCount(void) { ++operandCount; }
	void     add_to_machineCodeSize(unsigned int bytes)
			    { machineCodeSize += bytes; }
	mc_offset_t get_machineCodeSize(void) { return machineCodeSize; }
	void     add_parseStatus(statement_status_t status)
			    { parseStatus |= status; }
	int      get_machineCodeCols(void) { return machineCodeCols; }
	void     add_to_machineCodeCols(unsigned int cols)
			    { machineCodeCols += cols; }
	bool     isInstruction(void) { return isAnInstruction; }
	string   &get_textOpcode(void) { return textOpcode; }
	statement_status_t  get_parseStatus(void)
			    { return parseStatus; }
	string   &get_machineCode(void) { return machineCode; }
	void     set_sourceCode(const string &newStr) { sourceCode = newStr; }
	string   &get_sourceCode(void) { return sourceCode; }
	mc_offset_t get_sourceLines(void) { return sourceLines; }
	int     get_machineCodeFieldWidth(void) { return machineCodeFieldWidth; }

    // Allow access from derived classes
    protected:
	bool            isAnInstruction;
	statement_status_t  parseStatus;    // 0 if OK, bit codes for each error
	unsigned int    operandCount;
	string          sourceCode;
	string          machineCode;        // Text form of MC
	int             machineCodeFieldWidth;
	
    private:

	int             machineCodeCols;    // For alignment of source in list
	int             currentDataType;    // Most recent .byte, etc. directive
	mc_offset_t     sourceLines;        // Comments above + label + code
	mc_offset_t     endLabel;
	
	// Source code components
	string          label;              // empty if no label
	string          textOpcode;         // empty if not an instruction
	
	// Architecture-independent machine code components
	mc_offset_t     machineCodeSize;    // Size of machine code in bytes

	// Other machine code components stored in an architecture-dependent
	// derived class.
};

// Derived classes for specific architectures
#include "opcode.h"
#include "as68k.h"
#include "ecisc.h"
#include "riscv.h"

#endif

