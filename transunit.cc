#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sysexits.h>
#include <unistd.h>
#include "transunit.h"

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

TranslationUnit :: TranslationUnit()

{
    sourceLine = 0;
    codeOffset = 0;
    dataOffset = 0;
}


void    TranslationUnit :: openFiles(const char *sourceName)

{
    strlcpy(codeFileName, sourceName, PATH_MAX);
    strlcat(codeFileName, ".pass1-code", PATH_MAX);
    codePass1File.open(codeFileName,
	std::fstream::in | std::fstream::out | std::fstream::trunc);
    if ( codePass1File.fail() )
    {
	cerr << "Could not open " << codeFileName << ".\n";
	exit(EX_CANTCREAT);
    }

    strlcpy(dataFileName, sourceName, PATH_MAX);
    strlcat(dataFileName, ".pass1-data", PATH_MAX);
    dataPass1File.open(dataFileName,
	std::fstream::in | std::fstream::out | std::fstream::trunc);
    if ( codePass1File.fail() )
    {
	cerr << "Could not open " << codeFileName << ".\n";
	exit(EX_CANTCREAT);
    }
}


TranslationUnit :: ~TranslationUnit(void)

{
    codePass1File.close();
    dataPass1File.close();
    //unlink(codeFileName);
    //unlink(dataFileName);
}


void    TranslationUnit :: output_codeOffset(void)

{
    codePass1File << hex << setw(OFFSET_WIDTH) << setfill('0') << codeOffset << ' ';
}


void    TranslationUnit :: output_dataOffset(void)

{
    dataPass1File << hex << setw(OFFSET_WIDTH) << setfill('0') << dataOffset << ' ';
}


void    TranslationUnit :: errorMessage(const char *message, string sourceCode)

{
    cerr << "Line " << sourceLine << ": " << message << '\n';
    cerr << sourceCode << '\n';
}
