#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>    // sort()
#include <sysexits.h>
#include <regex.h>      // Needed for operand parsing?
#include "statement.h"

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
    
    cerr << binaryOpcode << endl;
    
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

