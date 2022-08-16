#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>    // sort()
#include <sysexits.h>
#include <regex.h>      // Operand parsing
#include "statement.h"

extern bool Debug;

using namespace std;

statement_riscv :: statement_riscv(void)

{
    /*
     *  Build a table of opcodes sorted lexically.  A binary
     *  search will be used to look up assembly opcodes and translate
     *  them to machine code.
     */
    initTable();

    // Sort with STL sort function, so we can use binary_search()
    // on the mnemonic later when translating.
    sort(opcodeTable.begin(), opcodeTable.end(), opcode_compare);

    // Debug
    cerr << "List of opcodes:\n";
    for (unsigned int c = 0; c < opcodeTable.size(); ++c)
	cerr 
	    << hex << setw(8) << setfill('0') << opcodeTable[c].get_bin()
	    << "  " << opcodeTable[c].get_assem() << '\n';
    cerr << '\n';
}


void    statement_riscv :: translateInstruction(string::size_type startPos)

{
}


/***************************************************************************
 * Description:
 *  Convert instruction to binary
 *
 * Arguments:
 *
 * Returns:
 *
 * History: 
 *  July 2022    J Bacon
 ***************************************************************************/

void statement_riscv :: translateOpcode(void)
{
    string opcode_sought = statement::get_textOpcode();
    opcode key(opcode_sought, 0);
    
    cerr << opcode_sought << endl;
    
    // Binary search opcode table
    vector<opcode>::const_iterator where =
	lower_bound(opcodeTable.begin(), opcodeTable.end(),
	key, opcode_compare);
    
    // Extract opcode
    string opcode_found = ((opcode)(*where)).get_assem();
    if ( opcode_found == statement::get_textOpcode() )
    {
	binaryOpcode = ((opcode)(*where)).get_bin();
	statement::add_to_machineCodeSize(1);   // 1 byte opcode
	statement::add_to_machineCodeCols(3);   // opcode + ' '
    }
    else
    {
	statement::add_parseStatus(STATEMENT_INVALID_OPCODE);
    }
    
    cerr << "Opcode: " << hex << setw(8) << binaryOpcode << endl;
    
    // Add opcode to machineInstruction
    machineInstruction = binaryOpcode;
}


/***************************************************************************
 * Description:
 *  Convert instruction to binary
 *
 * Arguments:
 *
 * Returns:
 *
 * History: 
 *  July 2022    J Bacon
 ***************************************************************************/

void    statement_riscv :: translateOperand(string &operand)

{
    static char const *pattern_reg_direct = "^[Xx][0-9][0-9]?$",
		*pattern_reg_indirect = "^\\([Xx][0-9][0-9]?\\)$",
		*pattern_numeric_offset = "^-?([0-9]+|0x[0-9a-fA-F]+)\\([Xx][0-9][0-5]?\\)$",
		*pattern_mem_direct = "^[a-zA-Z_][a-zA-Z0-9_]*$",
		*pattern_immediate_int = "^[0-9]+|0x[0-9a-fA-F]+$",
		*pattern_immediate_float = "^[0-9]*\\.[0-9]+(e[0-9]+)?$",
		*pattern_immediate_address = "^\\([0-9]+\\)|\\(0x[0-9a-fA-F]+\\)$";
    unsigned int        reg_num,
			operandCount = statement::get_operandCount();
    string::size_type   endOffset;
    // Boost is a huge and annoying dependency for just pattern matching
    // so we just use regex
    regex_t     preg_reg_direct,
		preg_reg_indirect,
		preg_numeric_offset,
		preg_mem_direct,
		preg_immediate_int,
		preg_immediate_float,
		preg_immediate_address;
    regmatch_t  matches[1];
 
    Debug = true;
    
    label[operandCount] = "";
    
    if (Debug) cerr << "Operand = " << operand << ' ';
    
    regcomp(&preg_reg_direct, pattern_reg_direct, REG_EXTENDED);
    regcomp(&preg_reg_indirect, pattern_reg_indirect, REG_EXTENDED);
    regcomp(&preg_numeric_offset, pattern_numeric_offset, REG_EXTENDED);
    regcomp(&preg_mem_direct, pattern_mem_direct, REG_EXTENDED);
    regcomp(&preg_immediate_int, pattern_immediate_int, REG_EXTENDED);
    regcomp(&preg_immediate_float, pattern_immediate_float, REG_EXTENDED);
    regcomp(&preg_immediate_address, pattern_immediate_address, REG_EXTENDED);
    
    // Register direct
    if ( regexec(&preg_reg_direct, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Reg direct\n";
	sscanf(operand.c_str(), "r%d", &reg_num);
    }
    
    // Register indirect
    else if ( regexec(&preg_reg_indirect, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Reg indirect\n";
	sscanf(operand.c_str(), "(r%d)", &reg_num);
    }
    
    // Numeric_offset
    else if ( regexec(&preg_numeric_offset, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Numeric offset\n";
	endOffset = operand.find("(");
	sscanf(operand.substr(endOffset).c_str(), "(x%d)", &reg_num);
	if (Debug) cerr << "reg_num = " << reg_num << endl;
	//cout << "Offset: " << label[operandCount] << ' ' << reg_num << '\n';
    }
    
    // Memory direct
    else if ( regexec(&preg_mem_direct, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Mem direct\n";
	label[operandCount] = operand;
    }
    
    // Immediate
    else if ( regexec(&preg_immediate_int, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Immediate int\n";
	sscanf(operand.c_str(), "%i", &operandValue[operandCount]);
    }
    
    // Immediate
    else if ( regexec(&preg_immediate_float, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Immediate float\n";
	sscanf(operand.c_str(), "%f", (float *)&operandValue[operandCount]);
    }
    
    else
	statement::add_parseStatus(STATEMENT_INVALID_OPERAND);

    regfree(&preg_reg_direct);
    regfree(&preg_reg_indirect);
    regfree(&preg_numeric_offset);
    regfree(&preg_mem_direct);
    regfree(&preg_immediate_int);
    regfree(&preg_immediate_float);
    regfree(&preg_immediate_address);
}


/***************************************************************************
 * Description:
 *
 * Arguments:
 *
 * Returns:
 *
 * History: 
 *
 ***************************************************************************/

bool statement_riscv :: isComment(string::size_type start_pos)

{
    string  &sourceCode = statement::get_sourceCode();
    string::size_type pos = sourceCode.find_first_not_of(" \t\n");
    
    if ( (pos == string::npos) || (sourceCode[pos] == '*') )
	return true;
    else
	return false;
}


void statement_riscv :: outputMl(ostream &outfile)

{
    outfile << hex << setw(8) << setfill('0') << machineInstruction;
}

