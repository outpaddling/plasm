#include <iostream>
#include <iomanip>
#include <string>
#include <sysexits.h>
#include "statement.h"

using namespace std;

/***************************************************************************
 * Description:
 *  Convert instruction to binary
 *
 * Arguments:
 *
 * Returns:
 *
 * History: 
 *  Nov 2009    J Bacon
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
 *  Nov 2009    J Bacon
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

