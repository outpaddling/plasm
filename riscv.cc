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
    labelOperand = "";

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


int     statement_riscv :: translateInstruction(TranslationUnit *transUnit,
						string::size_type startPos)

{
    stringstream        mc_stream;
    
    isAnInstruction = true;
    parseStatus = STATEMENT_OK;
    labelOperand = "";              // Set when mem_direct operand found

    // Translate opcode.  Needed to determine number and type of operands.
    translateOpcode(transUnit);
    
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
	// R-type
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
	    translateRtype(transUnit, startPos);
	    break;
	
	// I-type
	case    RISCV_OP_ADDI:
	case    RISCV_OP_SLLI:
	case    RISCV_OP_SLTI:
	case    RISCV_OP_SLTIU:
	case    RISCV_OP_XORI:
	case    RISCV_OP_SRLI:
	case    RISCV_OP_SRAI:
	case    RISCV_OP_ORI:
	case    RISCV_OP_ANDI:
	case    RISCV_OP_JALR:
	    translateItype(transUnit, startPos);
	    break;
	
	case    RISCV_OP_LB:
	case    RISCV_OP_LH:
	case    RISCV_OP_LW:
	case    RISCV_OP_LBU:
	case    RISCV_OP_LHU:
	case    RISCV_OP_LWU:
	case    RISCV_OP_LD:
	    translateLoad(transUnit, startPos);
	    break;
	
	// S-type
	case    RISCV_OP_SB:
	case    RISCV_OP_SH:
	case    RISCV_OP_SW:
	case    RISCV_OP_SD:
	    translateStype(transUnit, startPos);
	    break;
	
	// SB-type
	case    RISCV_OP_BEQ:
	case    RISCV_OP_BNE:
	case    RISCV_OP_BLT:
	case    RISCV_OP_BGE:
	case    RISCV_OP_BLTU:
	case    RISCV_OP_BGEU:
	    translateSBtype(transUnit, startPos);
	    break;
	default:
	    transUnit->errorMessage("Invalid opcode", sourceCode);
	    break;  // Silance warning
    }
    if ( Debug )
    {
	cerr << "Machine instruction = 0x" << hex << setw(8) << machineInstruction << '\n';
	binary_output(machineInstruction);
	cerr << '\n';
    }
    
    outputMl(transUnit, mc_stream);
    machineCode = mc_stream.str();
    
    return STATEMENT_OK;
}


int     statement_riscv :: translateRtype(TranslationUnit *transUnit,
					  string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits = 0;

    // First operand: rd
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected first operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("Expected register", sourceCode);
	    return STATEMENT_EXPECTED_REGISTER;
	}
	machineInstruction |= bits << 7;
    }

    // Second operand: rs1
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected second operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("Second operand is not a register.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 15;
    }
    
    // Third operand: rs2
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected third operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("Third operand is not a register.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 20;
    }
    
    return STATEMENT_OK;
}


int     statement_riscv :: translateItype(TranslationUnit *transUnit, string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits = 0;

    // First operand: rd
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected first operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("First operand is not a register", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 7;
    }

    // Second operand: rs1
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected second operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("Second operand is not a register.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 15;
    }
    
    // Third operand: immediate
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected third operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_IMMEDIATE )
	{
	    transUnit->errorMessage("Third operand is not an immediate.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 20;
    }
    
    return STATEMENT_OK;
}


int     statement_riscv :: translateLoad(TranslationUnit *transUnit, string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits = 0, reg_num, immediate;
    int                 mode;

    // First operand: rd
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected first operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("First operand is not a register.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 7;
    }

    // Second operand: address
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected second operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	mode = translateOperand(transUnit, text_operand, &bits);
	if ( mode == RISCV_MODE_OFFSET )
	{
	    // Bits contains immediate:rd in rightmost bits
	    reg_num = bits & 0x1f;
	    immediate = (bits >> 5) & 0xfff;
	    machineInstruction |= reg_num << 15 | immediate << 20;
	}
	else if ( mode == RISCV_MODE_LABEL )
	{
	    // Look up label and use offset-2048(gp)
	    // Do in second pass to allow forward refs
	}
	else
	{
	    transUnit->errorMessage("Second operand is not offset or label", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
    }
    
    return STATEMENT_OK;
}


int     statement_riscv :: translateStype(TranslationUnit *transUnit, string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits = 0, immediate7, immediate5, reg_num;
    int                 mode;

    // First operand: rs1
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected first operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("First operand is not a register.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 15;   // To rs1 field, not rd
    }

    // Second operand: address offset(rs2)
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected second operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
    
	// Validate operand using derived class?
	mode = translateOperand(transUnit, text_operand, &bits);
	if ( mode == RISCV_MODE_OFFSET )
	{
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
	else if ( mode == RISCV_MODE_LABEL )
	{
	    // Look up label and use offset-2048(gp)
	    // Do in second pass to allow forward refs
	}
	else
	{
	    transUnit->errorMessage("Second operand is not offset or label.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
    }

    return STATEMENT_OK;
}


int     statement_riscv :: translateSBtype(TranslationUnit *transUnit, string::size_type startPos)

{
    string              text_operand;
    string::size_type   start_operand,
			end_operand;
    uint64_t            bits = 0, immediate7, immediate5, reg_num;
    int                 mode;

    cerr << "Translating SB type.\n";
    
    // FIXME: Factor out sections like this from all translateXtype()
    // functions.  One function taking operand #, expected operand type,
    // and field position?
    
    // First operand: rs1
    start_operand = sourceCode.find_first_not_of(" \t\n", startPos);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected first operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);

	cerr << "First text_operand = " << text_operand << '\n';
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("First operand is not a register.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 15;   // To rs1 field, not rd
    }

    // Second operand: rs2
    // FIXME: Why doesn't translateRtype() need +1?
    start_operand = sourceCode.find_first_not_of(" \t\n", end_operand + 1);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected second operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);

	cerr << "Second text_operand = " << text_operand << '\n';
    
	// Validate operand using derived class?
	if ( translateOperand(transUnit, text_operand, &bits) != RISCV_MODE_REG_DIRECT )
	{
	    transUnit->errorMessage("Second operand is not a register.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
	machineInstruction |= bits << 7;   // To rs1 field, not rd
    }

    // Third operand: address
    start_operand = sourceCode.find_first_not_of(" \t,", end_operand);
    if ( (start_operand == string::npos) || isComment(start_operand) )
    {
	transUnit->errorMessage("Expected third operand.", sourceCode);
	return STATEMENT_OPERAND_COUNT;
    }
    else
    {
	end_operand = sourceCode.find_first_of(" \t,", start_operand);
	text_operand = sourceCode.substr(start_operand, end_operand-start_operand);
	
	cerr << "text_operand = " << text_operand << '\n';
    
	// Validate operand using derived class?
	mode = translateOperand(transUnit, text_operand, &bits);
	if ( mode == RISCV_MODE_OFFSET )
	{
	    // Bits contains immediate:rd in rightmost bits
	    // FIXME: Stored offset should be 1/2 actual
	    reg_num = (bits & 0x1f) << 20;          // rs2 field
	    // xxxxxxxxxxxxxxxiiiiiiiiiiiirrrrr     // bits
	    // iiiiiiirrrrrxxxxxxxxiiiiixxxxxxx     // S-type
	    // 00000000000000011111110000000000     // Upper 7 to funct7 field
	    immediate7 = (bits & 0x1fc00) << 15;
	    // 00000000000000000000001111100000     // Lower 5 to rd field
	    immediate5 = (bits & 0x3e0) << 2;
	    machineInstruction |= reg_num | immediate7 | immediate5;
	}
	else if ( mode == RISCV_MODE_LABEL )
	{
	    // Look up label and use offset-2048(gp)
	    // Do in second pass to allow forward refs
	}
	else
	{
	    transUnit->errorMessage("Third operand is not offset or label.", sourceCode);
	    return STATEMENT_INVALID_OPERAND;
	}
    }

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
 *  July 2022    J Bacon
 ***************************************************************************/

int  statement_riscv :: translateOpcode(TranslationUnit *transUnit)
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
	// All RISC V instructions are 32 bits
	statement::add_to_machineCodeSize(4);
    }
    else
	transUnit->errorMessage("Invalid opcode", sourceCode);
    
    if ( Debug ) cerr << "Opcode: " << hex << setw(8) << binary_opcode << '\n';
    
    // Add opcode to machineInstruction
    machineInstruction = binary_opcode;

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
 *  July 2022    J Bacon
 ***************************************************************************/

int     statement_riscv :: translateOperand(TranslationUnit *transUnit, string &operand, uint64_t *bits)

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
    
    // Memory direct: label
    else if ( regexec(&preg_mem_direct, operand.c_str(), 1, matches, 0) == 0 )
    {
	addressing_mode = RISCV_MODE_LABEL;
	labelOperand = operand;
	if ( Debug ) cerr << "Mem direct: " << operand << '\n';
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
	transUnit->errorMessage("Invalid operand", sourceCode);

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


void statement_riscv :: outputMl(TranslationUnit *transUnit, ostream &outfile)

{
    if ( labelOperand != "" )
	outfile << "@" << labelOperand << " ";
    outfile << hex << setw(8) << setfill('0') << machineInstruction;
}
