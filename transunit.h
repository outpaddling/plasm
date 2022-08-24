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
	
	inline uint32_t get_sourceLine(void) { return sourceLine; }
	inline void     set_sourceLine(uint32_t newLine) { sourceLine = newLine; }
	inline void     add_to_sourceLine(mc_offset_t n) { sourceLine += n; }
	
	inline uint32_t get_codeOffset(void) { return codeOffset; }
	inline fstream  &get_codeTempFile(void) { return codeTempFile; }
	inline void     add_to_codeOffset(mc_offset_t n) { codeOffset += n; }

	inline uint32_t get_dataOffset(void) { return dataOffset; }
	inline fstream  &get_dataTempFile(void) { return dataTempFile; }
	inline void     add_to_dataOffset(mc_offset_t n) { dataOffset += n; }

    private:
	mc_offset_t sourceLine;
	mc_offset_t codeOffset;
	mc_offset_t dataOffset;
	char        *codeFileName;
	char        *dataFileName;
	fstream     codeTempFile;
	fstream     dataTempFile;
};

#endif

