// *** lesson 1
#ifndef PHASE2_HPP
# define PHASE2_HPP
# include "../kocsymm/kocsymm.hpp"

// *** lesson 2
const int FACT8 = 40320;

class phase2
{
	public:

	static void init(int suppress_writing = 0);
	static int lookup(const cubepos &cp);
	static int lookup(const permcube &pc);

	// METHOD DECLARATION *** lesson 12

	static void gen_table();
	static int read_table();
	static void write_table();
	static void check_integrity();

	// *** lesson 24

	static moveseq solve(const permcube &pc, int maxlen = 30);

	static moveseq solve(const cubepos & cp, int maxlen = 30)
	{
		permcube pc(cp);
		return (solve(pc, maxlen));
	}

	static int solve(const permcube &pc, int togo, int canonstate, moveseq &seq);
	
	// DATA DECLARATION *** lesson 6

	static int cornermax;
	static unsigned int memsize;
	static unsigned int *mem;

	// *** lesson 17
	static const char *const filename;
	static int file_checksum;
};


#endif