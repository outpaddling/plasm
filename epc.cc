#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <sysexits.h>
#include <regex.h>
//#include <boost/regex.hpp>
#include "statement.h"

extern bool Debug;

using namespace std;

statement_epc :: statement_epc(void)

{
    /*
     *  Build a table of opcodes sorted lexically.  A binary
     *  search will be used to look up assembly opcodes and translate
     *  them to machine code.
     */
    #include "table_init.c"

    // Sort with STL sort function, so we can use binary_search()
    // on the mnemonic later when translating.
    sort(opcodeTable.begin(), opcodeTable.end(), opcode_compare);

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
 *  Convert opcode to binary
 *
 * Arguments:
 *
 * Returns:
 *
 * History: 
 *  May 2010    J Bacon
 ***************************************************************************/

void statement_epc :: translateOpcode(void)
{
    string opcode_sought = statement::get_textOpcode();
    opcode key(opcode_sought, 0);
    
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
    
    // Set data size for this instruction
    switch(binaryOpcode)
    {
	case    OP_MOVB:
	case    OP_CMPB:
	case    OP_NOTB:
	case    OP_ANDB:
	case    OP_ORB:
	case    OP_SLLB:
	case    OP_SRLB:
	case    OP_SRAB:
	case    OP_ROLB:
	case    OP_ADDB:
	case    OP_SUBB:
	case    OP_MULB:
	case    OP_DIVB:
	case    OP_REMB:
	case    OP_INPUTB:
	case    OP_OUTPUTB:
	    dataSize = 1;
	    break;
	case    OP_MOVS:
	case    OP_CMPS:
	case    OP_NOTS:
	case    OP_ANDS:
	case    OP_ORS:
	case    OP_SLLS:
	case    OP_SRLS:
	case    OP_SRAS:
	case    OP_ROLS:
	case    OP_ADDS:
	case    OP_SUBS:
	case    OP_MULS:
	case    OP_DIVS:
	case    OP_REMS:
	case    OP_INPUTS:
	case    OP_OUTPUTS:
	    dataSize = 2;
	    break;
	case    OP_MOVQ:
	case    OP_CMPQ:
	case    OP_NOTQ:
	case    OP_ANDQ:
	case    OP_ORQ:
	case    OP_SLLQ:
	case    OP_SRLQ:
	case    OP_SRAQ:
	case    OP_ROLQ:
	case    OP_ADDQ:
	case    OP_SUBQ:
	case    OP_MULQ:
	case    OP_DIVQ:
	case    OP_REMQ:
	case    OP_CMPD:
	case    OP_ADDD:
	case    OP_SUBD:
	case    OP_MULD:
	case    OP_DIVD:
	    dataSize = 8;
	    break;
	default:
	    /*
	     *  All other arithmetic instructions and all jump 
	     *  instructions use 4-byte operands.
	     */
	    dataSize = 4;
	    break;
    }
}


/***************************************************************************
 * Description:
 *  Convert operand to binary
 *
 * Arguments:
 *
 * Returns:
 *
 * History: 
 *  May 2010    J Bacon
 ***************************************************************************/

void    statement_epc :: translateOperand(string &operand)

{
    static char const *pattern_reg_direct = "^[Rr][0-9][0-5]?$",
		*pattern_reg_indirect = "^\\([Rr][0-9][0-5]?\\)$",
		*pattern_auto_increment = "^\\([Rr][0-9][0-5]?\\)\\+$",
		*pattern_auto_decrement = "^\\-\\([Rr][0-9][0-5]?\\)$",
		*pattern_label_offset = "^[a-zA-Z_][a-zA-Z0-9_]*\\([Rr][0-9][0-5]?\\)$",
		*pattern_numeric_offset = "^-?([0-9]+|0x[0-9a-fA-F]+)\\([Rr][0-9][0-5]?\\)$",
		*pattern_mem_direct = "^[a-zA-Z_][a-zA-Z0-9_]*$",
		*pattern_mem_indirect = "^\\([a-zA-Z_][a-zA-Z0-9_]*\\)$",
		*pattern_address_of = "^&[a-zA-Z_][a-zA-Z0-9_]*$",
		*pattern_immediate_int = "^[0-9]+|0x[0-9a-fA-F]+$",
		*pattern_immediate_float = "^[0-9]*\\.[0-9]+(e[0-9]+)?$",
		*pattern_immediate_address = "^\\([0-9]+\\)|\\(0x[0-9a-fA-F]+\\)$";
    unsigned int        reg_num,
			operandCount = statement::get_operandCount();
    string::size_type   endLabel, endOffset;
    // Boost is a huge and annoying dependency for just pattern matching
    // so we just use regex
    regex_t     preg_reg_direct,
		preg_reg_indirect,
		preg_auto_increment,
		preg_auto_decrement,
		preg_label_offset,
		preg_numeric_offset,
		preg_mem_direct,
		preg_mem_indirect,
		preg_address_of,
		preg_immediate_int,
		preg_immediate_float,
		preg_immediate_address;
    regmatch_t  matches[1];
    
    label[operandCount] = "";
    
    if (Debug) cerr << "Operand = " << operand << endl;
    
    regcomp(&preg_reg_direct, pattern_reg_direct, REG_EXTENDED);
    regcomp(&preg_reg_indirect, pattern_reg_indirect, REG_EXTENDED);
    regcomp(&preg_auto_increment, pattern_auto_increment, REG_EXTENDED);
    regcomp(&preg_auto_decrement, pattern_auto_decrement, REG_EXTENDED);
    regcomp(&preg_label_offset, pattern_label_offset, REG_EXTENDED);
    regcomp(&preg_numeric_offset, pattern_numeric_offset, REG_EXTENDED);
    regcomp(&preg_mem_direct, pattern_mem_direct, REG_EXTENDED);
    regcomp(&preg_mem_indirect, pattern_mem_indirect, REG_EXTENDED);
    regcomp(&preg_address_of, pattern_address_of, REG_EXTENDED);
    regcomp(&preg_immediate_int, pattern_immediate_int, REG_EXTENDED);
    regcomp(&preg_immediate_float, pattern_immediate_float, REG_EXTENDED);
    regcomp(&preg_immediate_address, pattern_immediate_address, REG_EXTENDED);
    
    // Register direct
    if ( regexec(&preg_reg_direct, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Reg direct\n";
	sscanf(operand.c_str(), "r%d", &reg_num);
	modeByte[operandCount] = MODE_REG_DIRECT | reg_num;
	statement::add_to_machineCodeSize(1);
	statement::add_to_machineCodeCols(3);
    }
    
    // Register indirect
    else if ( regexec(&preg_reg_indirect, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Reg indirect\n";
	sscanf(operand.c_str(), "(r%d)", &reg_num);
	modeByte[operandCount] = MODE_REG_INDIRECT | reg_num;
	statement::add_to_machineCodeSize(1);
	statement::add_to_machineCodeCols(3);
    }
    
    // Autoincrement
    else if ( regexec(&preg_auto_increment, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Autoincrement\n";
	sscanf(operand.c_str(), "(r%d)+", &reg_num);
	modeByte[operandCount] = MODE_AUTO_INCREMENT | reg_num;
	statement::add_to_machineCodeSize(1);
	statement::add_to_machineCodeCols(3);
    }
    
    // Autodecrement
    else if ( regexec(&preg_auto_decrement, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Autodecrement\n";
	sscanf(operand.c_str(), "-(r%d)", &reg_num);
	modeByte[operandCount] = MODE_AUTO_DECREMENT | reg_num;
	statement::add_to_machineCodeSize(1);
	statement::add_to_machineCodeCols(3);
    }
    
    // Numeric offset
    else if ( regexec(&preg_label_offset, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Num offset\n";
	if (Debug) cerr << "numeric offset operand = " << operand << '\n';
	endOffset = operand.find("(");
	if (Debug) cerr << endOffset << '\n';
	sscanf(operand.c_str(), "%i", &(operandValue[operandCount]));
	sscanf(operand.substr(endOffset).c_str(), "(r%d)", &reg_num);
	modeByte[operandCount] = MODE_OFFSET | reg_num;
	statement::add_to_machineCodeSize(1 + 4);
	statement::add_to_machineCodeCols(3 + 9);
    }
    
    // Label_offset
    else if ( regexec(&preg_numeric_offset, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Label offset\n";
	endLabel = operand.find("(");
	label[operandCount] = operand.substr(0, endLabel);
	sscanf(operand.substr(endLabel).c_str(), "(r%d)", &reg_num);
	modeByte[operandCount] = MODE_OFFSET | reg_num;
	statement::add_to_machineCodeSize(1 + 4);
	statement::add_to_machineCodeCols(3 + 10);
	//cout << "Offset: " << label[operandCount] << ' ' << reg_num << '\n';
    }
    
    // Memory direct
    else if ( regexec(&preg_mem_direct, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Mem direct\n";
	modeByte[operandCount] = MODE_MEM_DIRECT;
	statement::add_to_machineCodeSize(1 + 4);
	statement::add_to_machineCodeCols(3 + 10);
	label[operandCount] = operand;
    }
    
    // Memory indirect
    else if ( regexec(&preg_mem_indirect, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Mem indirect\n";
	modeByte[operandCount] = MODE_MEM_INDIRECT;
	statement::add_to_machineCodeSize(1 + 4);
	statement::add_to_machineCodeCols(3 + 10);
	label[operandCount] = operand.substr(1,operand.size()-2);
    }
    
    // Address of
    else if ( regexec(&preg_address_of, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Address of\n";
	modeByte[operandCount] = MODE_AUTO_INCREMENT | 15;
	statement::add_to_machineCodeSize(1 + 4);
	statement::add_to_machineCodeCols(3 + 10);
	label[operandCount] = operand.substr(1,operand.size()-1);
    }
    
    // Immediate
    else if ( regexec(&preg_immediate_int, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Immediate int\n";
	modeByte[operandCount] = MODE_AUTO_INCREMENT | 15;
	statement::add_to_machineCodeSize(1 + get_dataSize());
	statement::add_to_machineCodeCols(3 + get_dataSize() * 2 + 1);
	sscanf(operand.c_str(), "%i", &operandValue[operandCount]);
    }
    
    // Immediate
    else if ( regexec(&preg_immediate_float, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Immediate float\n";
	modeByte[operandCount] = MODE_AUTO_INCREMENT | 15;
	statement::add_to_machineCodeSize(1 + get_dataSize());
	statement::add_to_machineCodeCols(3 + get_dataSize() * 2 + 1);
	sscanf(operand.c_str(), "%f", (float *)&operandValue[operandCount]);
    }
    
    // Immediate address
    else if ( regexec(&preg_immediate_address, operand.c_str(), 1, matches, 0) == 0 )
    {
	if (Debug) cerr << "Immediate address\n";
	modeByte[operandCount] = MODE_MEM_DIRECT;
	statement::add_to_machineCodeSize(1 + 4);
	statement::add_to_machineCodeCols(3 + 9);
	sscanf(operand.c_str(), "(%i)", &operandValue[operandCount]);
	label[operandCount] = "";
    }
    
    else
	statement::add_parseStatus(STATEMENT_INVALID_OPERAND);

    regfree(&preg_reg_direct);
    regfree(&preg_reg_indirect);
    regfree(&preg_auto_increment);
    regfree(&preg_auto_decrement);
    regfree(&preg_label_offset);
    regfree(&preg_numeric_offset);
    regfree(&preg_mem_direct);
    regfree(&preg_mem_indirect);
    regfree(&preg_address_of);
    regfree(&preg_immediate_int);
    regfree(&preg_immediate_float);
    regfree(&preg_immediate_address);
}


/***************************************************************************
 * Description:
 *  Return true if everything after start_pos is whitespace or a comment
 *  starting with '#'.
 *
 * Arguments:
 *
 * Returns:
 *
 * History: 
 *
 ***************************************************************************/

bool statement_epc :: isComment(string::size_type start_pos)

{
    string  &sourceCode = statement::get_sourceCode();
    string::size_type pos = sourceCode.find_first_not_of(" \t\n", start_pos);
    
    if ( (pos == string::npos) || (sourceCode[pos] == '#') )
	return true;
    else
	return false;
}


void statement_epc :: outputMl(ostream &outfile)

{
    outfile << hex << setw(2) << setfill('0') <<
	(unsigned int)binaryOpcode << ' ';
    
    // Pseudo-instructions
    if ( (binaryOpcode == OP_MOVL) && (statement::get_textOpcode() == "ret") )
    {
	// Make sure no arguments given
	if ( statement::get_operandCount() != 0 )
	{
	    add_parseStatus(STATEMENT_OPERAND_COUNT);
	}
	
	// Output machine code for "(sp)+, pc"
	outfile << "2e 0f ";
	add_to_machineCodeCols(4);
	add_to_machineCodeSize(2);
    }
    
    for (unsigned int c = 0; c < statement::get_operandCount(); ++c)
    {
	// Output mode byte
	outfile << setw(2) << (unsigned int)modeByte[c] << ' ';
	
	// Output the rest of the operand for modes that have
	// more than a mode byte.
	switch(MODE_BITS(modeByte[c]))
	{
	    case    MODE_MEM_DIRECT:
	    case    MODE_MEM_INDIRECT:
	    case    MODE_OFFSET:
		if ( label[c] != "" )
		    outfile << '\002' << label[c] << ' ';
		else    // Immediate value or offset, no label
		    outfile << hex << setw(8) << setfill('0')
			<< operandValue[c] << ' ';
		break;
		
	    // Immediate mode
	    case    MODE_AUTO_INCREMENT:
		if ( REG_NUM(modeByte[c]) == 15 )
		{
		    if ( label[c] == "" )   // Immediate operand
		    {
			outfile << hex << setw(get_dataSize()*2)
				<< setfill('0')
				<< operandValue[c] << ' ';
		    }
		    else                    // Address of
		    {
			outfile << '\002' << label[c] << ' ';
		    }
		}
		break;
	}
    }

    // Pseudo-instructions
#if 0   // Old method before switching jumps to PC <- EA instead of PC <- OP
    if ( binaryOpcode == OP_MOVL )
    {
	if ( statement::get_textOpcode() == "j" )
	{
	    // Make sure only one argument given
	    if ( statement::get_operandCount() != 1 )
	    {
		add_parseStatus(STATEMENT_OPERAND_COUNT);
	    }
	    
	    // Output machine code for "pc"
	    outfile << "0f ";
	    add_to_machineCodeCols(3);
	    add_to_machineCodeSize(1);
	}
    }
#endif
}

