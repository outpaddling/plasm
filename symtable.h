#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <vector>
#include <string>

using namespace std;

#ifndef _TRANSUNIT_H_
#include "transunit.h"
#endif

#ifndef AVG
#define AVG(x,y)    (((x) + (y)) / 2)
#endif

#ifndef MAX
#define MAX(x,y)    ((x) > (y) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x,y)    ((x) < (y) ? (x) : (y))
#endif

class Symbol
{
    public:
	inline void set_name(string newName) { name = newName; }
	inline string &get_name(void) { return name; }
	inline void set_offset(mc_offset_t newOffset) { offset = newOffset; }
	inline mc_offset_t get_offset(void) { return offset; }
    private:
	mc_offset_t    offset;
	string      name;
	// int         type;
};


class SymTable
{
    public:
	int     addLabel(mc_offset_t offset, const string *name);
	void    sortByName(void);
	void    dump(ostream *outfile);
	Symbol  *lookupByName(string *name);
    private:
	vector<Symbol>  symbols;
};

#endif

