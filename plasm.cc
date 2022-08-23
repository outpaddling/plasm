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
    char            outfile_name[PATH_MAX+1], *p;
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
	strlcpy(outfile_name, argv[c], PATH_MAX);
	if ( (p = strrchr(outfile_name, '.')) != NULL )
	    strlcpy(p, ".bin", 5);
	else
	{
	    cerr << "Error: Source filename has no extension.\n"
		<< "Should be '.ecisc'.\n";
	    exit(EX_USAGE);
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
    
    /*
     *  Statement is a base class for statements of various architectures.
     */
    
    filename_extension = strrchr(filename, '.');
    if ( filename_extension != NULL )
    {
	if ( strcmp(filename_extension, ".ecisc") == 0 )
	    stmnt = new statement_ecisc;
	else if ( strcmp(filename_extension, ".68k") == 0 )
	    stmnt = new statement68k;
	else if ( strcmp(filename_extension, ".riscv") == 0 )
	    stmnt = new statement_riscv;
	else
	{
	    cerr << "Error: Unknown language: " << filename_extension << '\n';
	    cerr << "Supported extensions are \".ecisc\" and \".68k\".\n";
	    exit(EX_USAGE);
	}
    }
    else
    {
	cerr << "Error: filename must have an extension to indicate language.\n";
	cerr << "Supported extensions are \".ecisc\" and \".68k\".\n";
	exit(EX_USAGE);
    }

    /*
     *  Pass1: Build symbol table, translate
     */
    
    while ( stmnt->read(*infile) )
    {
	//string::size_type endLabel = 0;
	
	//cout << stmnt->get_sourceCode() << endl;
	
	transUnit.add_to_sourceLine(stmnt->get_sourceLines());
	//cerr << "Added " << stmnt->get_sourceLines() << " source lines.\n";
	
	if ( stmnt->isComment(0) )
	{
	    //cerr << "Comment: " << stmnt->get_sourceCode() << endl;
	    continue;
	}

	// Parse line into label, opcode, arguments
	stmnt->parse();
	
	if ( stmnt->get_parseStatus() != STATEMENT_OK )
	{
	    stmnt->printErrors(filename, transUnit.get_sourceLine());
	    assemStatus = EX_DATAERR;
	}

	// Output machine language
	if ( stmnt->isInstruction() )
	{
	    // If label present, add to symbol table
	    if ( stmnt->hasLabel() )
		codeSymTable.addLabel(transUnit.get_codeOffset(), &stmnt->get_label());
	
	    // Offset for list file
	    transUnit.output_codeOffset();
	    transUnit.get_codeTempFile() << stmnt->get_machineCode();
	    transUnit.add_to_codeOffset(stmnt->get_machineCodeSize());

	    // Align source code
	    for (int c = 0;
		c < stmnt->get_machineCodeFieldWidth() - stmnt->get_machineCodeCols(); ++c)
		transUnit.get_codeTempFile() << ' ';
	    
	    // Source code
	    transUnit.get_codeTempFile() << "# " << stmnt->get_sourceCode() << endl;
	}
	else
	{
	    if ( stmnt->hasLabel() )
		dataSymTable.addLabel(transUnit.get_dataOffset(),
				    &stmnt->get_label());
	    
	    transUnit.output_dataOffset();
	    transUnit.get_dataTempFile() << stmnt->get_machineCode();
	    transUnit.add_to_dataOffset(stmnt->get_machineCodeSize());

	    // Align source code
	    for (int c = 0;
		c < stmnt->get_machineCodeFieldWidth() - stmnt->get_machineCodeCols(); ++c)
		transUnit.get_dataTempFile() << ' ';
	    
	    // Source code
	    transUnit.get_dataTempFile() << "# " << stmnt->get_sourceCode() << endl;
	}
    }
    
    codeSymTable.sortByName();
    dataSymTable.sortByName();
    
    /*
     *  Pass2: Resolve symbol references
     */
    
    if ( assemStatus == EX_OK )
    {
	transUnit.get_codeTempFile().seekg(0, ios::beg);
	transUnit.get_dataTempFile().seekg(0, ios::beg);
	
	char    ch;
	// Replace label with address
	string  label;
	Symbol  *match;
    
	while ( transUnit.get_codeTempFile().get(ch) )
	{
	    if ( ch != '\002' )
		(*outfile).put(ch);
	    else
	    {
		transUnit.get_codeTempFile() >> label;
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
    
	(*outfile) << '\n';
	mc_offset_t    offset;
	while ( transUnit.get_dataTempFile() >> offset )
	{
	    (*outfile) << setw(OFFSET_WIDTH) 
		<< transUnit.get_codeOffset() + offset;
	    while ( transUnit.get_dataTempFile().get(ch) && (ch != '\n') )
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
