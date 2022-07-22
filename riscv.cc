#include <iostream>
#include <iomanip>
#include <string>
#include <sysexits.h>
#include "statement.h"

using namespace std;

statement_riscv :: statement_riscv(void)

{
    /*
     *  Build a table of opcodes sorted lexically.  A binary
     *  search will be used to look up assembly opcodes and translate
     *  them to machine code.
     */
    #include "riscv-table-init.c"

    // Sort with STL sort function, so we can use binary_search()
    // on the mnemonic later when translating.
    // sort(opcodeTable.begin(), opcodeTable.end(), opcode_compare);

    // Debug
    /*
    for (unsigned int c = 0; c < opcodeTable.size(); ++c)
	cout << setw(6) << setfill(' ') << opcodeTable[c].get_assem() << ' '
	    << hex << setw(2) << setfill('0') << opcodeTable[c].get_bin()
	    << '\n';
    */
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
    outfile << hex << setw(4) << setfill('0') << instruction;
}

