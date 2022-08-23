#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cctype>
//#define __STDC_FORMAT_MACROS    // OS X Snow Leopard gcc 4.2.1 SCNu64
#include <inttypes.h>
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
    string::size_type   startOpcode,
			endOpcode;

    parseStatus = STATEMENT_OK;
    
    /*
     *  Parsing out the opcode and operands, and building the symbol table
     *  is the same for all assembly languages, so it's handled by the base
     *  class.
     */
    
    cerr << "\nSource code: " << sourceCode << '\n';
    
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
    
    if ( textOpcode[0] == '.' )
	processDirective(endOpcode);
    else
	translateInstruction(endOpcode);
    return;
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
    else
	add_parseStatus(STATEMENT_INVALID_DIRECTIVE);
    
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
    stringstream        mc_stream;
    string::size_type   val_start,
			val_end,
			repeat_pos;
    string              val_text,
			repeat_str;
    char                *end;
    int                 repeat,
			c,
			initializers = 0;
    uint64_t            integer;
    float               float_val;
    double              double_val;
    
    val_end = startPos + 1; // Prime loop
    
    while ( (val_start = sourceCode.find_first_of("0123456789\"", val_end))
	    != string::npos )
    {
	++initializers;
	
	if ( sourceCode[val_start] == '"' )
	    val_end = sourceCode.find('"', val_start+1) + 1;
	else
	    val_end = sourceCode.find_first_of(" \t,", val_start+1);
	
	val_text = sourceCode.substr(val_start, val_end-val_start);
	if ( (repeat_pos=val_text.find(':')) != string::npos )
	{
	    ++repeat_pos;
	    repeat_str = val_text.substr(repeat_pos, val_text.size()-repeat_pos);
	    sscanf(repeat_str.c_str(), "%i", &repeat);
	}
	else
	    repeat = 1;
	
	mc_stream << hex << setfill('0');

	switch(currentDataType)
	{
	    case    TYPE_BYTE:
		//sscanf(val_text.c_str(), "%"SCNu64, &integer);
		integer = strtoull(val_text.c_str(), &end, 10);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(2) << integer << ' ';
		    machineCodeCols += 3;
		    ++machineCodeSize;
		}
		break;
	    
	    case    TYPE_SHORT:
		//sscanf(val_text.c_str(), "%"SCNu64, &integer);
		integer = strtoull(val_text.c_str(), &end, 10);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(4) << integer << ' ';
		    machineCodeCols += 5;
		    machineCodeSize += 2;
		}
		break;
	    
	    case    TYPE_LONG:
		//sscanf(val_text.c_str(), "%"SCNu64, &integer);
		integer = strtoull(val_text.c_str(), &end, 10);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(8) << integer << ' ';
		    machineCodeCols += 9;
		    machineCodeSize += 4;
		}
		break;
	    
	    case    TYPE_QUAD:
		//sscanf(val_text.c_str(), "%"SCNu64, &integer);
		integer = strtoull(val_text.c_str(), &end, 10);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(16) << integer << ' ';
		    machineCodeCols += 17;
		    machineCodeSize += 8;
		}
		break;
	    
	    case    TYPE_FLOAT:
		sscanf(val_text.c_str(), "%f", &float_val);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(8) << *(uint32_t *)&float_val << ' ';
		    machineCodeCols += 9;
		    machineCodeSize += 4;
		}
		break;
	    
	    case    TYPE_DOUBLE:
		sscanf(val_text.c_str(), "%lf", &double_val);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(16) << *(uint32_t *)&double_val << ' ';
		    machineCodeCols += 17;
		    machineCodeSize += 8;
		}
		break;
	    
	    case    TYPE_STRING:
		//mc_stream << val_text << '\n';
		for (const char *p = val_text.c_str() + 1; *p != '"'; ++p)
		{
		    machineCodeSize += 1;
		    machineCodeCols += 3;
		    if ( *p != '\\' )
			mc_stream << (int)*p;
		    else
		    {
			++p;
			switch(*p)
			{
			    case    '0':
				mc_stream << setw(2) << 0;
				break;
			    
			    case    'a':
				mc_stream << setw(2) << 7;
				break;
			    
			    case    'b':
				mc_stream << setw(2) << 8;
				break;
			    
			    case    't':
				mc_stream << setw(2) << 9;
				break;
			    
			    case    'n':
				mc_stream << setw(2) << 10;
				break;
			    
			    case    'f':
				mc_stream << setw(2) << 12;
				break;
			    
			    case    'r':
				mc_stream << setw(2) << 13;
				break;
			    default:
				mc_stream << setw(2) << (int)*p;
				break;
			}
		    }
		    mc_stream << ' ';
		}
		break;
	}
    }
    machineCode = mc_stream.str();
    
    if ( initializers == 0 )
	add_parseStatus(STATEMENT_MISSING_INITIALIZER);
}


void    statement :: printErrors(const char *filename,
				unsigned long line)

{
    cerr << dec << filename << ", line " << line << ": ";
    cerr << "parseStatus = " << parseStatus << ": ";
    
    if ( parseStatus & STATEMENT_MISSING_COLON )
	cerr << "Label detected (text in column 1), but no colon found.\n";
    
    if ( parseStatus & STATEMENT_BAD_LABEL )
	cerr << "Label contains illegal characters.  Only letters, digits, and '_' allowed.\n";

    if ( parseStatus & STATEMENT_INVALID_OPCODE )
	cerr << "Invalid opcode.\n";

    if ( parseStatus & STATEMENT_INVALID_OPERAND )
	cerr << "Invalid operand.\n";

    if ( parseStatus & STATEMENT_MISSING_INITIALIZER )
	cerr << "Missing initializer.\n";

    if ( parseStatus & STATEMENT_OPERAND_COUNT )
	cerr << "Wrong number of arguments for instruction.\n";

    if ( parseStatus & STATEMENT_INVALID_DIRECTIVE )
	cerr << "Invalid directive.\n";

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
    ++sourceLines;
    return infile;
}
