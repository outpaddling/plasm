#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include "symtable.h"


int     SymTable :: addLabel(mc_offset_t offset, const string *name)

{
    Symbol  newSymbol;
    
    //cerr << "Adding " << *name << " at 0x" << hex << setw(4) << setfill('0') 
    //    << offset << '\n';
    
    newSymbol.set_name(*name);
    newSymbol.set_offset(offset);
    symbols.push_back(newSymbol);
    return 0;
}


bool symbol_compare(const Symbol &sym1, const Symbol &sym2)

{
    return ((Symbol)sym1).get_name() < ((Symbol)sym2).get_name();
}


void    SymTable :: sortByName(void)

{
    sort(symbols.begin(), symbols.end(), symbol_compare);
}


void    SymTable :: dump(ostream *outfile)

{
    for (unsigned int c = 0; c < symbols.size(); ++c)
	(*outfile) << hex << setw(8) << setfill('0') <<
	    symbols[c].get_offset() << ' ' << symbols[c].get_name() << '\n';
}


Symbol  *SymTable :: lookupByName(string *name)

{
    int     first = 0,
	    last = symbols.size()-1,
	    c;
    
    do
    {
	c = AVG(first, last);
	/*
	cout << "Comparing " << symbols[c].get_name() << " and " <<
	    *name << endl;
	cout << "c = " << c << endl;
	*/
	
	if ( symbols[c].get_name() == *name )
	    return &symbols[c];
	else if ( *name > symbols[c].get_name() )
	    first = c + 1;
	else
	    last = c - 1;
    }   while ( first <= last );
    
    return NULL;
}

