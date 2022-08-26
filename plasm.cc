#include <iostream>
#include <iomanip>
#include <fstream>
#include <sysexits.h>
#include <string>
#include <cstdlib>
#include <cstring>
#include <climits>
#include "statement.h"
#include "symtable.h"
#include "plasm.h"

bool Debug = false;

int     main(int argc,char *argv[])

{
    ifstream        infile;
    ofstream        outfile;
    char            outfile_name[PATH_MAX+1];
    int             c;
    
    for (c = 1; (c < argc) && (*argv[c] == '-'); ++c)
    {
	cerr << argv[c] << endl;
	if ( strcmp(argv[c], "--debug") == 0 )
	{
	    Debug = true;
	    cerr << "Debug on.\n";
	}
    }
    
    //process_args(argc, argv);
    if ( argv[c] == NULL )
    {
	return assem(argv[0], "standard-input", &cin, &cout);
    }
    else
    {
	/* Open source file */
	infile.open(argv[c]);
	if ( infile.fail() )
	{
	    cerr << "Failed to open " << argv[c] << ".\n";
	    return EX_UNAVAILABLE;
	}
	
	/* Open target file */
	if ( (strstr(argv[c], ".ecisc") == NULL) &&
	     (strstr(argv[c], ".riscv") == NULL) )
	{
	    cerr << "Error: Source filename has no extension.\n"
		<< "Should be '.ecisc' or '.riscv'.\n";
	    exit(EX_USAGE);
	}
	else
	{
	    strlcpy(outfile_name, argv[c], PATH_MAX);
	    strlcat(outfile_name, ".bin", PATH_MAX);
	}
	outfile.open(outfile_name);
	if ( outfile.fail() )
	{
	    cerr << "Failed to open " << outfile_name << ".\n";
	    return EX_UNAVAILABLE;
	}

	/* Assemble source */
	return assem(argv[0], argv[c], &infile, &outfile);
    }
}


int     assem(const char *prog_name, const char *filename,
	    istream *infile, ostream *outfile)

{
    TranslationUnit transUnit;
    SymTable        codeSymTable;
    SymTable        dataSymTable;
    statement       *stmnt;
    const char      *filename_extension;
    int             assemStatus = EX_OK;
    mc_offset_t     offset;
    
    /*
     *  Statement is a base class for statements of various architectures.
     */
    
    filename_extension = strrchr(filename, '.');
    if ( filename_extension != NULL )
    {
	if ( strcmp(filename_extension, ".ecisc") == 0 )
	    stmnt = new statement_ecisc;
	else if ( strcmp(filename_extension, ".riscv") == 0 )
	    stmnt = new statement_riscv;
	else
	{
	    cerr << "Error: Unknown language: " << filename_extension << '\n';
	    cerr << "Supported extensions are \".ecisc\" and \".riscv\".\n";
	    exit(EX_USAGE);
	}
    }
    else
    {
	cerr << "Error: filename must have an extension to indicate language.\n";
	cerr << "Supported extensions are \".ecisc\" and \".riscv\".\n";
	exit(EX_USAGE);
    }

    transUnit.openFiles(filename);
    
    /*
     *  Pass1: Build symbol table, translate
     */

    // Read next statement, which may span multiple lines, e.g. if label
    // and code are not on the same line
    while ( stmnt->read(&transUnit, *infile) )
    {
	transUnit.add_to_sourceLine(stmnt->get_sourceLines());
	
	if ( stmnt->isComment(0) )
	    continue;

	// Parse line into label, opcode, arguments
	stmnt->parse(&transUnit);
	
	if ( stmnt->get_parseStatus() != STATEMENT_OK )
	    assemStatus = EX_DATAERR;

	// Output machine language
	if ( stmnt->isInstruction() )
	{
	    // If label present, add to symbol table
	    if ( stmnt->hasLabel() )
		codeSymTable.addLabel(transUnit.get_codeOffset(),
				      &stmnt->get_label());
	
	    // Output code offset and machine code
	    transUnit.output_codeOffset();
	    transUnit.get_codePass1File() << stmnt->get_machineCode();
	    
	    transUnit.add_to_codeOffset(stmnt->get_machineCodeSize());

	    // Pad with spaces to align source code to the right
	    // ECISC has variable size instructions
	    for (int c = 0;
		c < stmnt->get_machineCodeFieldWidth() 
		    - stmnt->get_machineCodeCols(); ++c)
		transUnit.get_codePass1File() << ' ';
	    
	    // Output source code
	    transUnit.get_codePass1File() << "# " << stmnt->get_sourceCode() << endl;
	}
	else
	{
	    if ( stmnt->hasLabel() )
		dataSymTable.addLabel(transUnit.get_dataOffset(),
				    &stmnt->get_label());
	    
	    transUnit.output_dataOffset();
	    transUnit.get_dataPass1File() << stmnt->get_machineCode();
	    
	    transUnit.add_to_dataOffset(stmnt->get_machineCodeSize());

	    // Align source code
	    for (int c = 0;
		c < stmnt->get_machineCodeFieldWidth() - stmnt->get_machineCodeCols(); ++c)
		transUnit.get_dataPass1File() << ' ';
	    
	    // Source code
	    transUnit.get_dataPass1File() << "# " << stmnt->get_sourceCode() << endl;
	}
    }
    
    codeSymTable.sortByName();
    dataSymTable.sortByName();
    
    /*
     *  Pass2: Resolve symbol references
     *  ECISC: Label marked by '\002' for each operand.
     *      Need a char that cannot otherwise appear in the pass1 output
     *
     *  FIXME: Add RISC-V support.
     *      All instruction codes exactly 32 bits
     *      @Label precedes hex instruction code
     *
     *  FIXME: Factor out some of this code
     */
    
    if ( assemStatus == EX_OK )
    {
	char    ch;
	// Replace label with address
	string  label;
	Symbol  *match;
	
	transUnit.get_codePass1File().seekg(0, ios::beg);
	transUnit.get_dataPass1File().seekg(0, ios::beg);
	
	// Labels are embedded differently in ECISC and RISC-V
	if ( strcmp(filename_extension, ".ecisc") == 0 )
	{
	    while ( transUnit.get_codePass1File().get(ch) )
	    {
		if ( ch != '\002' )
		    (*outfile).put(ch);
		else
		{
		    transUnit.get_codePass1File() >> label;
		    match = codeSymTable.lookupByName(&label);
		    if ( match != NULL )
			(*outfile) << '@' << hex << setw(8) << setfill('0') <<
			    match->get_offset();
		    else
		    {
			match = dataSymTable.lookupByName(&label);
			if ( match != NULL )
			    (*outfile) << '@' << hex << setw(8) << setfill('0') <<
				match->get_offset() + transUnit.get_codeOffset();
			else
			    cerr << "\nError: label \"" << label << "\" undefined.\n";
		    }
		}
	    }
	}
	else if ( strcmp(filename_extension, ".riscv") == 0 )
	{
	    while ( transUnit.get_codePass1File() >> offset )
	    {
		(*outfile) << hex << setw(OFFSET_WIDTH) << setfill('0')
		    << offset;
	    
		// FIXME: Now check for label and convert to an offset
		// in the machine code
		while ( transUnit.get_codePass1File().get(ch) && (isspace(ch)) )
		    (*outfile).put(ch);
		
		if ( ch == '@' )
		{
		    // Offset within text or data segment
		    mc_offset_t data_offset = 0, code_offset = 0;
		    // 12 or 20 bit immediate field
		    int         imm_offset = 0;
		    uint32_t    machine_code, opcode;
		    
		    transUnit.get_codePass1File() >> label;
		    cerr << "Label found: " << label << '\n';
		    match = codeSymTable.lookupByName(&label);
		    if ( match != NULL )
		    {
			code_offset = match->get_offset();
			cerr << "Code offset = " << code_offset << '\n';
		    }
		    else
		    {
			// Offset of symbol in data segment
			match = dataSymTable.lookupByName(&label);
			if ( match != NULL )
			{
			    data_offset = match->get_offset();
			    if ( data_offset > 4096 )
				cerr << "Error: Data segment > 4096 bytes.\n";
			    cerr << "Data offset = " 
				 << hex << setw(8) << setfill('0')
				 << data_offset << '\n';
			}
			else
			    cerr << "\nError: label \"" << label << "\" undefined.\n";
		    }

		    // Note: Data segment is first in source, but second
		    // in machine code
		    transUnit.get_codePass1File() >> machine_code;
		    opcode = machine_code & 0x7f;
		    //(*outfile) << " hello ";
		    switch(opcode)
		    {
			case    RISCV_OP_BEQ & 0x7f:    // Any branch
			    // Debug
			    (*outfile) << '@'
				    << hex << setw(8) << setfill('0')
				    << match->get_offset();
			    break;
			
			// Load instructions: GP-relateive (x3)
			case    RISCV_OP_LB & 0x7f:
			    imm_offset = ((int)data_offset - 2048) & 0xfff;
			    machine_code |= imm_offset << 20;
			    machine_code |= 3 << 15;            // gp
			    (*outfile) << hex << setw(8) << setfill('0')
				<< machine_code;

			    // Debug
			    cerr << "Immediate offset = " << imm_offset << '\n';
			    cerr << "Load instruction with machine_code: "
				 << machine_code << '\n';
			    binary_output(machine_code);
			    cerr << '\n';
			    break;
			
			case    RISCV_OP_SB & 0x7f:     // Any store
			    imm_offset = ((int)data_offset - 2048) & 0xfff;
			    // S-type: offset7 rs1 rs2 funct3 offset5 opcode
			    // Offset5
			    machine_code |= (imm_offset & 0x1f) << 7;
			    // Offset7: Already shifted 5, so 25 - 5 = 20
			    machine_code |= (imm_offset & 0xfe0) << 20;
			    machine_code |= 3 << 15;            // gp = x3
			    (*outfile) << hex << setw(8) << setfill('0')
				<< machine_code;

			    // Debug
			    cerr << "Immediate offset = " << imm_offset << '\n';
			    cerr << "Store instruction with machine_code: "
				 << machine_code << '\n';
			    binary_output(machine_code);
			    cerr << '\n';
			    break;
			
			case    RISCV_OP_JALR & 0x7f:   // PC-relative
			    // Debug
			    (*outfile) << '@'
				    << hex << setw(8) << setfill('0')
				    << match->get_offset();
			    break;
			
			case    RISCV_OP_JAL & 0x7f:
			    // Debug
			    (*outfile) << '@'
				    << hex << setw(8) << setfill('0')
				    << match->get_offset();
			    break;
			
			default:
			    cerr << "Unsupported opcode for label: "
				 << hex << opcode << '\n';
			    break;
		    }
		}
		else
		    (*outfile).put(ch); // First char of machine code
		
		// Copy rest of line
		while ( transUnit.get_codePass1File().get(ch) && (ch != '\n') )
		    (*outfile).put(ch);
		(*outfile).put(ch); // Newline
	    }
	}
	(*outfile) << '\n';

	// Data segment
	while ( transUnit.get_dataPass1File() >> offset )
	{
	    (*outfile) << hex << setw(OFFSET_WIDTH) << setfill('0')
		<< transUnit.get_codeOffset() + offset;
	    while ( transUnit.get_dataPass1File().get(ch) && (ch != '\n') )
		(*outfile).put(ch);
	    (*outfile) << '\n';
	}

	(*outfile) << "End\n";
	(*outfile) << "\nCode symbols:\n";
	codeSymTable.dump(outfile);
	(*outfile) << "\nData symbols:\n";
	dataSymTable.dump(outfile);
    }
    return assemStatus;
}


void    usage(char *argv[])

{
    cerr << "Usage: " << argv[0] << "[-d] [source-file]\n";
    exit(EX_USAGE);
}


void    binary_output(uint32_t n)

{
    int     c;
    
    for (c = 0; c < 32; ++c, n <<= 1)
	if ( n & 0x80000000 )
	    cerr << '1';
	else
	    cerr << '0';
}
