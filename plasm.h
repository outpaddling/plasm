
#ifndef _PLASM_H_
#define _PLASM_H_

using namespace std;

#define MAX_LINE_LEN    1024

int     assem(const char *prog_name, const char *filename, istream *infile, ostream *outfile);
void    usage(char *argv[]);

#endif

