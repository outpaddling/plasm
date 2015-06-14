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
    
    codeFileName = strdup("/tmp/codefileXXXXXX");
    mkstemp(codeFileName);
    codeTempFile.open(codeFileName);
    if ( codeTempFile.fail() )
    {
	cerr << "Could not open " << codeFileName << ".\n";
	exit(EX_CANTCREAT);
    }

    dataFileName = strdup("/tmp/datafileXXXXXX");
    mkstemp(dataFileName);
    dataTempFile.open(dataFileName);
    if ( codeTempFile.fail() )
    {
	cerr << "Could not open " << codeFileName << ".\n";
	exit(EX_CANTCREAT);
    }
}


TranslationUnit :: ~TranslationUnit(void)

{
    codeTempFile.close();
    dataTempFile.close();
    unlink(codeFileName);
    unlink(dataFileName);
}


void    TranslationUnit :: output_codeOffset(void)

{
    codeTempFile << hex << setw(OFFSET_WIDTH) << setfill('0') << codeOffset << ' ';
}


void    TranslationUnit :: output_dataOffset(void)

{
    dataTempFile << hex << setw(OFFSET_WIDTH) << setfill('0') << dataOffset << ' ';
}

