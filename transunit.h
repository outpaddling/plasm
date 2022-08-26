#ifndef _TRANSUNIT_H_
#define _TRANSUNIT_H_

#include <fstream>
#include <string>
#include <stdint.h>

using namespace std;

#define OFFSET_WIDTH    8   /* Hex display width */

typedef unsigned long   mc_offset_t;

class TranslationUnit
{
    public:
	TranslationUnit(void);
	~TranslationUnit(void);
	void            output_codeOffset(void);
	void            output_dataOffset(void);
	
	uint32_t get_sourceLine(void) { return sourceLine; }
	void     set_sourceLine(uint32_t newLine) { sourceLine = newLine; }
	void     add_to_sourceLine(mc_offset_t n) { sourceLine += n; }
	
	uint32_t get_codeOffset(void) { return codeOffset; }
	fstream  &get_codePass1File(void) { return codePass1File; }
	void     add_to_codeOffset(mc_offset_t n) { codeOffset += n; }

	uint32_t get_dataOffset(void) { return dataOffset; }
	fstream &get_dataPass1File(void) { return dataPass1File; }
	void    add_to_dataOffset(mc_offset_t n) { dataOffset += n; }
	void    errorMessage(const char *message, string sourceCode);
	void    openFiles(const char *sourceName);

    private:
	mc_offset_t sourceLine;
	mc_offset_t codeOffset;
	mc_offset_t dataOffset;
	char        codeFileName[PATH_MAX];
	char        dataFileName[PATH_MAX];
	fstream     codePass1File;
	fstream     dataPass1File;
};

#endif

