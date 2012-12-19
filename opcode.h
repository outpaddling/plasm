#include <string>

using namespace std;

class opcode
{
    public:
	opcode(void);
	opcode(const char *new_assem, uint32_t new_bin);
	opcode(const string &new_assem, uint32_t new_bin);
	inline string   &get_assem(void) { return assem; }
	inline uint32_t get_bin(void) { return bin; }
    private:
	string      assem;
	uint32_t    bin;
};

bool opcode_compare(const opcode &op1, const opcode &op2);

