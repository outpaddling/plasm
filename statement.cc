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

extern bool Debug;

void statement :: parse(TranslationUnit *transUnit)

{
    string::size_type   startOpcode,
			endOpcode;

    parseStatus = STATEMENT_OK;
    
    /*
     *  Parsing out the opcode and operands, and building the symbol table
     *  is the same for all assembly languages, so it's handled by the base
     *  class.
     */
    
    if ( Debug ) cerr << "\nSource code: " << sourceCode << '\n';
    
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
	processDirective(transUnit, endOpcode);
    else
	translateInstruction(transUnit, endOpcode);
    return;
}


void    statement :: processDirective(TranslationUnit *transUnit,
				      string::size_type startPos)

{
    isAnInstruction = false;
    
    /*
     *  If more directives are added, it may be worth setting up
     *  a searchable table at some point.  Until then, just do a linear
     *  search on the strings.
     */
    
    if ( textOpcode == ".int8" )
	currentDataType = TYPE_INT8;
    else if ( textOpcode == ".string" )
	currentDataType = TYPE_STRING;
    else if ( textOpcode == ".int16" )
	currentDataType = TYPE_INT16;
    else if ( textOpcode == ".int32" )
	currentDataType = TYPE_INT32;
    else if ( textOpcode == ".int64" )
	currentDataType = TYPE_INT64;
    else if ( textOpcode == ".float32" )
	currentDataType = TYPE_FLOAT32;
    else if ( textOpcode == ".float64" )
	currentDataType = TYPE_FLOAT64;
    else
	transUnit->errorMessage("Invalid directive", sourceCode);
    
    processInitializers(transUnit, startPos);
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

void    statement :: processInitializers(TranslationUnit *transUnit,
					 string::size_type startPos)

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
	    case    TYPE_INT8:
		//sscanf(val_text.c_str(), "%"SCNu64, &integer);
		integer = strtoull(val_text.c_str(), &end, 10);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(2) << integer << ' ';
		    machineCodeCols += 3;
		    ++machineCodeSize;
		}
		break;
	    
	    case    TYPE_INT16:
		//sscanf(val_text.c_str(), "%"SCNu64, &integer);
		integer = strtoull(val_text.c_str(), &end, 10);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(4) << integer << ' ';
		    machineCodeCols += 5;
		    machineCodeSize += 2;
		}
		break;
	    
	    case    TYPE_INT32:
		//sscanf(val_text.c_str(), "%"SCNu64, &integer);
		integer = strtoull(val_text.c_str(), &end, 10);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(8) << integer << ' ';
		    machineCodeCols += 9;
		    machineCodeSize += 4;
		}
		break;
	    
	    case    TYPE_INT64:
		//sscanf(val_text.c_str(), "%"SCNu64, &integer);
		integer = strtoull(val_text.c_str(), &end, 10);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(16) << integer << ' ';
		    machineCodeCols += 17;
		    machineCodeSize += 8;
		}
		break;
	    
	    case    TYPE_FLOAT32:
		sscanf(val_text.c_str(), "%f", &float_val);
		for (c = 0; c < repeat; ++c)
		{
		    mc_stream << setw(8) << *(uint32_t *)&float_val << ' ';
		    machineCodeCols += 9;
		    machineCodeSize += 4;
		}
		break;
	    
	    case    TYPE_FLOAT64:
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
	transUnit->errorMessage("Missing initializer", sourceCode);
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


istream    &statement :: read(TranslationUnit *transUnit, istream &infile)

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
		transUnit->errorMessage("Bad label", sourceCode);
	}
	else
	    transUnit->errorMessage("Missing colon", sourceCode);
	
	// Skip forward to next line with an opcode
	++endLabel;
	while ( isComment(endLabel) && getline(infile, sourceCode) )
	    endLabel = 0;
    }
    ++sourceLines;
    return infile;
}
