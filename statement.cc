#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cctype>
#include "statement.h"

/*
 *  For efficiency, we minimize the use of string operators such as
 *  [], and try to make the optimized string class member functions
 *  do most of the work.
 */

/***************************************************************************
 * Description:
 *  Parse a statement in any assembly language.
 *
 * Arguments:
 *  sourceCode:   string containing an entire line of text.
 *
 * Returns:
 *
 * History: 
 *
 ***************************************************************************/

void statement :: parse(void)

{
    parseStatus = STATEMENT_OK;
    string::size_type   startOpcode,
			endOpcode;

    /*
     *  Parsing out the opcode and operands, and building the symbol table
     *  is the same for all assembly languages, so it's handled by the base
     *  class.
     */
    
    // Initialize for new instruction
    machineCodeSize = 0;
    machineCodeCols = 0;
    machineCode = "";
    
    // Extract text opcode
    startOpcode = sourceCode.find_first_not_of(" \t", endLabel+1);
    if ( startOpcode == string::npos )
	return;
    endOpcode = sourceCode.find_first_of(" \t", startOpcode+1);
    textOpcode = sourceCode.substr(startOpcode, endOpcode-startOpcode);
    
    if ( textOpcode[0] != '.' )
    {
	translateInstruction(endOpcode);
    }
    else
    {
	processDirective(endOpcode);
    }
    return;
}


void    statement :: translateInstruction(string::size_type startPos)

{
    stringstream        mcStream;
    string              textOperand;
    string::size_type   startOperand,
			endOperand;
    
    isAnInstruction = true;
    parseStatus = STATEMENT_OK;

    // Translate opcode.  Needed to determine number and type of arguments.
    translateOpcode();
    
    /*
     *  This operand parsing is architecture-independent.  This isn't
     *  the most efficient implementation, since the architecture-specific
     *  operand translator will parse each operand a second time.  However,
     *  it shouldn't cause any noticeable performance issues, and it
     *  helps minimize code redundancy by letting the base class do
     *  as much of the parsing as possible.
     */
    
    // Parse out operands
    operandCount = 0;
    startOperand = sourceCode.find_first_not_of(" \t\n", startPos);
    while ( (operandCount < MAX_OPERANDS) &&
	    (startOperand != string::npos) &&
	    ! isComment(startOperand) )
    {
	endOperand = sourceCode.find_first_of(" \t,", startOperand);
	textOperand = sourceCode.substr(startOperand, endOperand-startOperand);
    
	// Validate operand using derived class?
	translateOperand(textOperand);
	
	// Next operand
	++operandCount;
	startOperand = sourceCode.find_first_not_of(" \t,", endOperand);
    }
    
    // Fixme: Get rid of outputMl and make the translate member functions
    // build the string directly.
    outputMl(mcStream);
    machineCode = mcStream.str();
}


void    statement :: processDirective(string::size_type startPos)

{
    isAnInstruction = false;
    
    /*
     *  If more directives are added, it may be worth setting up
     *  a searchable table at some point.  Until then, just do a linear
     *  search on the strings.
     */
    
    if ( textOpcode == ".byte" )
	currentDataType = TYPE_BYTE;
    else if ( textOpcode == ".string" )
	currentDataType = TYPE_STRING;
    else if ( textOpcode == ".short" )
	currentDataType = TYPE_SHORT;
    else if ( textOpcode == ".long" )
	currentDataType = TYPE_LONG;
    else if ( textOpcode == ".quad" )
	currentDataType = TYPE_QUAD;
    else if ( textOpcode == ".float" )
	currentDataType = TYPE_FLOAT;
    else if ( textOpcode == ".double" )
	currentDataType = TYPE_DOUBLE;

    processInitializers(startPos);
}


/***************************************************************************
 *  Description:
 *  
 *  Arguments:
 *
 *  Returns:
 *
 *  History: 
 *  Date        Name        Modification
 *
 ***************************************************************************/

void    statement :: processInitializers(string::size_type startPos)

{
    stringstream        mcStream;
    string::size_type   valStart,
			valEnd,
			repeatPos;
    string              valText,
			repeatStr;
    int                 repeat,
			c,
			initializers = 0;
    uint64_t            integer;
    float               float_val;
    double              double_val;
    
    valEnd = startPos + 1; // Prime loop
    
    //cout << sourceCode << endl;
    while ( (valStart = sourceCode.find_first_of("0123456789\"", valEnd))
	    != string::npos )
    {
	++initializers;
	
	if ( sourceCode[valStart] == '"' )
	    valEnd = sourceCode.find('"', valStart+1) + 1;
	else
	    valEnd = sourceCode.find_first_of(" \t,", valStart+1);
	
	valText = sourceCode.substr(valStart, valEnd-valStart);
	if ( (repeatPos=valText.find(':')) != string::npos )
	{
	    ++repeatPos;
	    repeatStr = valText.substr(repeatPos, valText.size()-repeatPos);
	    sscanf(repeatStr.c_str(), "%i", &repeat);
	}
	else
	    repeat = 1;
	
	mcStream << hex << setfill('0');

	switch(currentDataType)
	{
	    case    TYPE_BYTE:
		sscanf(valText.c_str(), "%qi", &integer);
		for (c = 0; c < repeat; ++c)
		{
		    mcStream << setw(2) << integer << ' ';
		    machineCodeCols += 3;
		    ++machineCodeSize;
		}
		break;
	    
	    case    TYPE_SHORT:
		sscanf(valText.c_str(), "%qi", &integer);
		for (c = 0; c < repeat; ++c)
		{
		    mcStream << setw(4) << integer << ' ';
		    machineCodeCols += 5;
		    machineCodeSize += 2;
		}
		break;
	    
	    case    TYPE_LONG:
		sscanf(valText.c_str(), "%qi", &integer);
		for (c = 0; c < repeat; ++c)
		{
		    mcStream << setw(8) << integer << ' ';
		    machineCodeCols += 9;
		    machineCodeSize += 4;
		}
		break;
	    
	    case    TYPE_QUAD:
		sscanf(valText.c_str(), "%qi", &integer);
		for (c = 0; c < repeat; ++c)
		{
		    mcStream << setw(16) << integer << ' ';
		    machineCodeCols += 17;
		    machineCodeSize += 8;
		}
		break;
	    
	    case    TYPE_FLOAT:
		sscanf(valText.c_str(), "%f", &float_val);
		for (c = 0; c < repeat; ++c)
		{
		    mcStream << setw(8) << *(uint32_t *)&float_val << ' ';
		    machineCodeCols += 9;
		    machineCodeSize += 4;
		}
		break;
	    
	    case    TYPE_DOUBLE:
		sscanf(valText.c_str(), "%lf", &double_val);
		for (c = 0; c < repeat; ++c)
		{
		    mcStream << setw(16) << *(uint32_t *)&double_val << ' ';
		    machineCodeCols += 17;
		    machineCodeSize += 8;
		}
		break;
	    
	    case    TYPE_STRING:
		//mcStream << valText << '\n';
		for (const char *p = valText.c_str() + 1; *p != '"'; ++p)
		{
		    machineCodeSize += 1;
		    machineCodeCols += 3;
		    if ( *p != '\\' )
			mcStream << (int)*p;
		    else
		    {
			++p;
			switch(*p)
			{
			    case    '0':
				mcStream << setw(2) << 0;
				break;
			    
			    case    'a':
				mcStream << setw(2) << 7;
				break;
			    
			    case    'b':
				mcStream << setw(2) << 8;
				break;
			    
			    case    't':
				mcStream << setw(2) << 9;
				break;
			    
			    case    'n':
				mcStream << setw(2) << 10;
				break;
			    
			    case    'f':
				mcStream << setw(2) << 12;
				break;
			    
			    case    'r':
				mcStream << setw(2) << 13;
				break;
			    default:
				mcStream << setw(2) << (int)*p;
				break;
			}
		    }
		    mcStream << ' ';
		}
		break;
	}
    }
    machineCode = mcStream.str();
    
    if ( initializers == 0 )
	add_parseStatus(STATEMENT_MISSING_INITIALIZER);
}


void    statement :: printErrors(const char *filename,
				unsigned long line)

{
    cerr << dec << filename << ", line " << line << ": ";
    
    if ( parseStatus & STATEMENT_MISSING_COLON )
	cerr << "Label detected (text in column 1), but no colon found.\n";
    
    if ( parseStatus & STATEMENT_BAD_LABEL )
	cerr << "Label contains illegal characters.  Only letters, digits, and '_' allowed.\n";

    if ( parseStatus & STATEMENT_INVALID_OPERAND )
	cerr << "Invalid operand.\n";

    if ( parseStatus & STATEMENT_MISSING_INITIALIZER )
	cerr << "Missing initializer.\n";

    if ( parseStatus & STATEMENT_OPERAND_COUNT )
	cerr << "Wrong number of arguments for instruction.\n";

    cerr << sourceCode << '\n';
}


bool statement :: hasLabel(void)
{
    return ! label.empty();
}


bool    statement :: badLabel(void)

{
    const char    *str = label.c_str();
    
    if ( !isalpha(str[0]) )
	return true;
    for (int c=1; str[c] != '\0'; ++c)
	if ( !isalpha(str[c]) && !isdigit(str[c]) && (str[c] != '_') )
	    return true;
    return false;
}


istream    &statement :: read(istream &infile)

{
    sourceLines = 0;
    endLabel = 0;
    label = "";
    
    while ( getline(infile, sourceCode) && isComment(0) )
	++sourceLines;

    // Extract label and validate if it exists.  Must begin in col 1.
    char    first_ch = sourceCode[0];
    if ( !isspace(first_ch) && (first_ch != '\0') )
    {
	endLabel = sourceCode.find(':', 0);
	if ( endLabel != string::npos )
	{
	    set_label(sourceCode.substr(0, endLabel));
	    
	    // Validate label structure
	    if ( badLabel() )
		add_parseStatus(STATEMENT_BAD_LABEL);
	}
	else
	    add_parseStatus(STATEMENT_MISSING_COLON);
	
	// Skip forward to next line with an opcode
	++endLabel;
	while ( isComment(endLabel) && getline(infile, sourceCode) )
	    endLabel = 0;
    }
    return infile;
}

