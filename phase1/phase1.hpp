// *** lesson 1
// Phase 1 involves finding a sequence of move that takes an arbitrary position into the H group.
// We do a pruning table to have exact dept of the current position to eliminate most of the false path
// The depth can range from 0 (already in H) to 12.
#ifndef PHASE1_HPP
# define PHASE1_HPP
# include "../kocsymm/kocsymm.hpp"

// CONSTANT DECLARATION *** lesson 6
const int BYTES_PER_ENTRY = 4;

class phase1
{
	public:

	static void init(int suppress_writing = 0);
	static int lookup(const kocsymm &kc, int &mask);

	// METHOD DECLARATION *** lesson 8
	static void gen_table();
	static int read_table();
	static void write_table();
	static void check_integrity();

	// *** lesson 16
	static int lookup(const kocsymm &kc);
	static int lookup(const kocsymm &kc, int togo, int &nextmovemask);
	static moveseq solve(kocsymm kc);

	// DATA DECLARATION *** lesson 3
	static unsigned int memsize;
	static unsigned char *mem;
	static int file_checksum;
	static const char *const filename;
};

#endif