#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>    // sort()
#include <sysexits.h>
#include <regex.h>      // Operand parsing
#include "statement.h"
#include "plasm.h"

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
    
    machineCodeFieldWidth = 16;

    // Sort with STL sort function, so we can use binary_search()
    // on the mnemonic later when translating.
    sort(opcodeTable.begin(), opcodeTable.end(), opcode_compare);

    // Debug
    if ( Debug ) 
    {
	cerr << "List of opcodes:\n";
	for (unsigned int c = 0; c < opcodeTable.size(); ++c)
	    cerr 
		<< hex << setw(8) << setfill('0') << opcodeTable[c].get_bin()
		<< "  " << opcodeTable[c].get_assem() << '\n';
	cerr << '\n';
    }
}


void    statement_riscv :: translateInstruction(string::size_type startPos)

{
    stringstream        mc_stream;
    
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
    
    switch(machineInstruction)
    {
	case    RISCV_OP_ADD:
	case    RISCV_OP_SUB:
	case    RISCV_OP_XOR:
	case    RISCV_OP_OR:
	case    RISCV_OP_AND:
	case    RISCV_OP_SLL:
	case    RISCV_OP_SRL:
	case    RISCV_OP_SRA:
	case    RISCV_OP_SLT:
	case    RISCV_OP_SLTU:
	    translateRtype(startPos);
	    break;
	case    RISCV_OP_ADDI:
	    translateItype(startPos);
	    break;
	case    RISCV_OP_LD:
	    translateLoad(startPos);
	    break;
	case    RISCV_OP_SD:
	    translateStype(startPos);
	    break;
	default:
	    cerr << "translateInstruction: Invalid opcode "
		 << machineInstruction << '\n';
	    exit(EX_DATAERR);
	    break;  // Silance warning
    }
    if ( Debug )
    {
	cerr << "Machine instruction = 0x" << hex << setw(8) << machineInstruction << '\n';
	binary_output(machineInstruction);
	cerr << '\n';
    }
    
    outputMl(mc_stream);
    machineCode = mc_stream.str();
}


void    statement_riscv :: translateRtype(string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits;

    // First operand: rd
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateRtype(): Expected first operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    cerr << "translateRtype(): First operand is not a register.\n";
	}
	machineInstruction |= bits << 7;
    }

    // Second operand: rs1
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateRtype(): Expected second operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    cerr << "translateRtype(): Second operand is not a register.\n";
	}
	machineInstruction |= bits << 15;
    }
    
    // Third operand: rs2
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateRtype(): Expected third operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    cerr << "translateRtype(): Third operand is not a register.\n";
	}
	machineInstruction |= bits << 20;
    }
}


void    statement_riscv :: translateItype(string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits;

    // First operand: rd
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateItype(): Expected first operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    cerr << "translateItype(): First operand is not a register.\n";
	}
	machineInstruction |= bits << 7;
    }

    // Second operand: rs1
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateItype(): Expected second operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    cerr << "translateItype(): Second operand is not a register.\n";
	}
	machineInstruction |= bits << 15;
    }
    
    // Third operand: immediate
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateItype(): Expected third operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_IMMEDIATE )
	{
	    cerr << "translateItype(): Third operand is not an immediate.\n";
	}
	machineInstruction |= bits << 20;
    }
}


void    statement_riscv :: translateLoad(string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits, reg_num, immediate;

    // First operand: rd
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateLoad(): Expected first operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    cerr << "translateLoad(): First operand is not a register.\n";
	}
	machineInstruction |= bits << 7;
    }

    // Second operand: address
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateLoad(): Expected second operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_OFFSET )
	{
	    cerr << "translateLoad(): Second operand is not offset.\n";
	}
	// Bits contains immediate:rd in rightmost bits
	reg_num = bits & 0x1f;
	immediate = (bits >> 5) & 0xfff;
	machineInstruction |= reg_num << 15 | immediate << 20;
    }
}


void    statement_riscv :: translateStype(string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits, immediate7, immediate5, reg_num;

    // First operand: rd
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateStype(): Expected first operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    cerr << "translateStype(): First operand is not a register.\n";
	}
	machineInstruction |= bits << 15;   // To rs1 field, not rd
    }

    // Second operand: address
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	cerr << "translateStype(): Expected second operand.\n";
	return;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(text_operand, &bits) != RISCV_MODE_OFFSET )
	{
	    cerr << "translateStype(): Second operand is not offset.\n";
	}
	// Bits contains immediate:rd in rightmost bits
	reg_num = (bits & 0x1f) << 20;          // rs2 field
	// xxxxxxxxxxxxxxxiiiiiiiiiiiirrrrr     // bits
	// iiiiiiirrrrrxxxxxxxxiiiiixxxxxxx     // S-type
	// 00000000000000011111110000000000     // Upper 7 to funct7 field
	immediate7 = (bits & 0x1fc00) << 15;
	// 00000000000000000000001111100000     // Lower 5 to rd field
	immediate5 = (bits & 0x3e0) << 2;
	machineInstruction |= reg_num | immediate7 | immediate5;
    }
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
    string      text_opcode = statement::get_textOpcode();
    opcode      key(text_opcode, 0);
    uint32_t    binary_opcode = 0;
    
    if ( Debug ) cerr << "Text opcode: " << text_opcode << '\n';
    
    // Binary search opcode table
    vector<opcode>::const_iterator where =
	lower_bound(opcodeTable.begin(), opcodeTable.end(),
	key, opcode_compare);
    
    // Extract opcode
    string opcode_found = ((opcode)(*where)).get_assem();
    if ( opcode_found == statement::get_textOpcode() )
    {
	binary_opcode = ((opcode)(*where)).get_bin();
	statement::add_to_machineCodeSize(1);   // 1 byte opcode
	statement::add_to_machineCodeCols(3);   // opcode + ' '
    }
    else
	statement::add_parseStatus(STATEMENT_INVALID_OPCODE);
    
    if ( Debug ) cerr << "Opcode: " << hex << setw(8) << binary_opcode << '\n';
    
    // Add opcode to machineInstruction
    machineInstruction = binary_opcode;
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

int     statement_riscv :: translateOperand(string &operand, uint64_t *bits)

{
    static char const *pattern_reg_direct = "^[Xx][0-9][0-9]?$",
		*pattern_reg_indirect = "^\\([Xx][0-9][0-9]?\\)$",
		*pattern_numeric_offset = "^-?([0-9]+|0x[0-9a-fA-F]+)\\([Xx][0-9][0-5]?\\)$",
		*pattern_mem_direct = "^[a-zA-Z_][a-zA-Z0-9_]*$",
		*pattern_immediate_int = "^-?[0-9]+|0x[0-9a-fA-F]+$",
		*pattern_immediate_float = "^-?[0-9]*\\.[0-9]+(e[0-9]+)?$",
		*pattern_immediate_address = "^\\([0-9]+\\)|\\(0x[0-9a-fA-F]+\\)$";
    unsigned int        reg_num;
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
    int         int_operand;
    double      float_operand;
    int         addressing_mode = RISCV_MODE_UNKNOWN;
 
    if ( Debug ) cerr << "Operand = " << operand << ' ';
    
    // FIXME: Do this once and store in the class object
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
	addressing_mode = RISCV_MODE_REG_DIRECT;
	sscanf(operand.c_str(), "x%d", &reg_num);
	if ( Debug ) cerr << "Register direct: " << reg_num << '\n';
	if ( (reg_num >= 0) && (reg_num <= 31) )
	    *bits = reg_num;
	else
	    cerr << "Invalid register: " << reg_num << '\n';
    }
    
    // Register indirect
    else if ( regexec(&preg_reg_indirect, operand.c_str(), 1, matches, 0) == 0 )
    {
	sscanf(operand.c_str(), "(x%d)", &reg_num);
	if ( Debug ) cerr << "Register indirect: " << reg_num << '\n';
    }
    
    // Numeric_offset
    else if ( regexec(&preg_numeric_offset, operand.c_str(), 1, matches, 0) == 0 )
    {
	addressing_mode = RISCV_MODE_OFFSET;
	if ( Debug ) cerr << "Numeric offset\n";
	sscanf(operand.c_str(), "%i(x%d)", &int_operand, &reg_num);
	if ( Debug ) cerr << "Offset: " << int_operand << ", " << reg_num << '\n';
	if ( (int_operand >= -2048) && (int_operand <= 2047) )
	{
	    if ( (reg_num >= 0) && (reg_num <= 31) )
		*bits = reg_num | int_operand << 5;
	    else
		cerr << "Invalid register: " << reg_num << '\n';
	}
	else
	{
	    cerr << "Invalid offset: " << int_operand << '\n';
	    cerr << "Range is -2048 to +2047.\n";
	}
    }
    
    // Memory direct
    // label(register) or label
    else if ( regexec(&preg_mem_direct, operand.c_str(), 1, matches, 0) == 0 )
    {
	// string branch_label = operand;
	if ( Debug ) cerr << "Mem direct\n";
    }
    
    // Immediate
    else if ( regexec(&preg_immediate_int, operand.c_str(), 1, matches, 0) == 0 )
    {
	addressing_mode = RISCV_MODE_IMMEDIATE;
	sscanf(operand.c_str(), "%i", &int_operand);
	if ( Debug ) cerr << "Immediate int: " << int_operand << '\n';
	if ( (int_operand >= -2048) && (int_operand <= 2047) )
	    *bits = int_operand;
	else
	{
	    cerr << "Immediate operand out of range: " << int_operand << '\n';
	    cerr << "Limits are -2048 to +2047.\n";
	    *bits = 0;
	}
    }
    
    // Immediate
    else if ( regexec(&preg_immediate_float, operand.c_str(), 1, matches, 0) == 0 )
    {
	sscanf(operand.c_str(), "%lf", &float_operand);
	if ( Debug ) cerr << "Immediate float: " << float_operand << '\n';
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
    
    return addressing_mode;
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

