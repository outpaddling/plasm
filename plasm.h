
#ifndef _PLASM_H_
#define _PLASM_H_

using namespace std;

#ifdef __linux__
#define strlcpy(dest, source, size) strcpy(dest, source)
#define strlcat(dest, source, size) strcat(dest, source)
#endif

#define MAX_LINE_LEN    1024

int     assem(const char *prog_name, const char *filename, istream *infile, ostream *outfile);
void    usage(char *argv[]);
void    binary_output(uint32_t n);

#endif

