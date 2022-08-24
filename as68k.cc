#include <iostream>
#include <iomanip>
#include <string>
#include <sysexits.h>
#include "statement.h"

using namespace std;

int     statement68k :: translateInstruction(string::size_type startPos)

{
    return STATEMENT_OK;
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

int     statement68k :: translateOpcode(void)
{
    return STATEMENT_OK;
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

int     statement68k :: translateOperand(string &operand, uint64_t *bits)

{
    return 0;
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

bool statement68k :: isComment(string::size_type start_pos)

{
    string  &sourceCode = statement::get_sourceCode();
    string::size_type pos = sourceCode.find_first_not_of(" \t\n");
    
    if ( (pos == string::npos) || (sourceCode[pos] == '*') )
	return true;
    else
	return false;
}


void statement68k :: outputMl(ostream &outfile)

{
    outfile << hex << setw(4) << setfill('0') << instruction;
}

