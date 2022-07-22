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
	void            parse(void);
	void            translateInstruction(string::size_type endLabel);
	void            processDirective(string::size_type endLabel);
	void            processInitializers(string::size_type endLabel);
	void            printErrors(const char *filename,
				    unsigned long line);

	bool            hasLabel(void);
	bool            badLabel(void);

	// Architecture-dependent: Must be implemented by derived class
	virtual void    translateOpcode(void) = 0;
	virtual void    translateOperand(string &operand) = 0;
	virtual bool    isComment(string::size_type start_post) = 0;
	virtual void    outputMl(ostream &outfile) = 0;

	// Mutators, accessors, etc.
	inline unsigned int    get_operandCount(void) { return operandCount; }
	inline string   &get_label(void) { return label; }
	inline void     set_label(const string &newLabel) { label = newLabel; }
	inline void     increment_operandCount(void) { ++operandCount; }
	inline void     add_to_machineCodeSize(unsigned int bytes)
			    { machineCodeSize += bytes; }
	inline mc_offset_t get_machineCodeSize(void) { return machineCodeSize; }
	inline void     add_parseStatus(statement_status_t status)
			    { parseStatus |= status; }
	inline int      get_machineCodeCols(void) { return machineCodeCols; }
	inline void     add_to_machineCodeCols(unsigned int cols)
			    { machineCodeCols += cols; }
	inline bool     isInstruction(void) { return isAnInstruction; }
	inline string   &get_textOpcode(void) { return textOpcode; }
	inline statement_status_t  get_parseStatus(void)
			    { return parseStatus; }
	inline string   &get_machineCode(void) { return machineCode; }
	inline void     set_sourceCode(const string &newStr) { sourceCode = newStr; }
	inline string   &get_sourceCode(void) { return sourceCode; }
	inline mc_offset_t get_sourceLines(void) { return sourceLines; }
    
    private:
	bool            isAnInstruction;
	statement_status_t  parseStatus;    // 0 if OK, bit codes for each error

	unsigned int    operandCount;
	int             machineCodeCols;    // For alignment of source in list
	int             currentDataType;    // Most recent .byte, etc. directive
	mc_offset_t     sourceLines;        // Comments above + label + code
	mc_offset_t     endLabel;
	
	// Source code components
	string          label;              // empty if no label
	string          textOpcode;         // empty if not an instruction
	string          sourceCode;
	
	// Architecture-independent machine code components
	string          machineCode;        // Text form of MC
	mc_offset_t     machineCodeSize;    // Size of machine code in bytes

	// Other machine code components stored in an architecture-dependent
	// derived class.
};

// Derived classes for specific architectures
#include "opcode.h"
#include "as68k.h"
#include "epc.h"
#include "riscv.h"

#endif

