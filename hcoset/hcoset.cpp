// *** lesson 1
const char *BANNER = "This is hcoset 1.1 (C) 2010-2012 Tomas Rokicki, All Rights Reserved.";
#include "../phase1/phase1.hpp"
#include <pthread.h>
#include <iostream>
#include <map>
#include <set>
#include <cstdio>
// DATA DECLARATION *** lesson 2
int verbose = 1;
int numthreads = 1;
const int MAX_THREAD = 32;
// *** lesson 4
// this serve to avoid writing the pruning table if it doesn't exists
int skipwrite = 0;
// *** lesson 6
// we need to find as many phase1 solution to a particolar group as it can, because every
// phase 1 solution leads to a particolar coset. We start with the simplier coset solver
// in a set
moveseq repseq;
std::set<permcube> world;
kocsymm repkc;
permcube reppc;
cubepos repcp;
// *** lesson 10
// level of slowness, 0 slower, 1 normal, 2 fast
int slow = 2;
// maximum level to search, this help terminate the program
int maxsearchdepht = 35;
// overall maximum level
int maxdepth = 35;
// global depht
int global_depth;
// *** lesson 15
// pagination for the memory
const int FACT8 = 40320;
const int PAGESIZE = (FACT8 * FACT4 / 2 / 8);
unsigned char **bitp1, **bitp2;
// *** lesson 17
long long uniq = 0;
long long probes = 0;
const long long TARGET = FACT8 * (long long) FACT8 * (long long) FACT4 / 2;
#ifdef LEVELCOUNTS
	long long uniq_ulev = 0;
	long long sum_ulev[30];
#endif
// *** lesson 19
// a small array to indicate which pages were touched
#ifdef FASTCLEAN
	unsigned char touched[FACT8];
	int did_a_prepass;
#endif
// lesson 21
// lesson 23
// lesson 29
// lesson 32
// lesson 35
// lesson 39
// lesson 43
// lesson 44
// lesson 49
// lesson 56
// lesson 61
// lesson 66
// lesson 75
// lesson 76
// lesson 79
// lesson 82

// UTILITY FUNCTION *** lesson 13
// slow search routine
void slowsearch1(const kocsymm &kc, const permcube &pc, int togo, int movemask, int canon)
{
	if (togo == 0)
	{
		if (kc == identity_kc)
		{
			++probes;
			world.insert(pc);
		}
		return;
	}
	--togo;
	kocsymm kc2;
	permcube pc2;
	int newmovemask;
	while (movemask)
	{
		int mv = ffs(movemask) - 1;
		movemask &= movemask - 1;
		kc2 = kc;
		kc2.move(mv);
		int nd = phase1::lookup(kc2, togo, newmovemask);
		if (nd <= togo)
		{
			pc2 = pc;
			pc2.move(mv);
			int new_canon = cubepos::next_cs(canon, mv);
			slowsearch1(kc2, pc2, togo, newmovemask & cubepos::cs_mask(new_canon), new_canon);
		}
	}
}
// *** lesson 14
// this is the actual search, it will never end with this logic.
void slowsearch1(const kocsymm &kc, const permcube &pc)
{
	duration();
	for (int d = phase1::lookup(repkc); d < maxsearchdepht; ++d)
	{
		probes = 0;
		long long prevlev = uniq;
		slowsearch1(kc, pc, d, ALLMOVEMASK, CANONSEQSTART);
		uniq = world.size();
		long long thislev = uniq - prevlev;
		if (verbose)
			std::cout << "Tests at " << d << " " << probes << " in " << duration() << " uniq " << uniq << " lev " << thislev << std::endl;
	}
}
// *** lesson 18
// method to get a cleared page
std::vector<unsigned char *> pageq;
unsigned char *getpage()
{
	unsigned char *r = 0;
	if (pageq.size() > 0)
	{
		r = pageq[pageq.size() - 1];
		pageq.pop_back();
	}
	else
	{
		r = (unsigned char *) malloc (PAGESIZE + 1);
		if (r == 0)
			error("! no memory");
	}
	return (r);
}
unsigned char *getclearedpage()
{
	unsigned char *r = getpage();
	memset(r, 0, PAGESIZE);
	return (r);
}
void freepage(unsigned char *r)
{
	pageq.push_back(r);
}
// lesson 24
// lesson 25
// lesson 27
// lesson 28
// lesson 37
// lesson 41
// lesson 42
// lesson 45
// lesson 46
// lesson 51
// lesson 52
int singcount;
// lesson 55
// lesson 57
// lesson 58
// lesson 59
// lesson 60
// lesson 62
// lesson 69
// lesson 78
// lesson 80
// lesson 83
// lesson 85

// THREADING OBJECT lesson 48

void docoset(int seq, const char *movestring)
{
	// handle one coset from movestring *** lesson 8
	// parse the movestring
	int oldsingcount = singcount;
	const char *tmp = movestring;
	repseq = cubepos::parse_moveseq(tmp);
	if (*tmp)
		error("! extra stuff at end of input moveseq");
	std::cout << "Coset representative " << seq << " " << movestring << std::endl;
	phase1::init(skipwrite);
	double cosetstart = waltime();
	// *** lesson 9
	// we execute the move sequence on repkc
	repkc = identity_kc;
	reppc = identity_pc;
	repcp = identity_cube;
	for (unsigned int i = 0; i < repseq.size(); ++i)
	{
		repkc.move(repseq[i]);
		reppc.move(repseq[i]);
		repcp.move(repseq[i]);
	}
	#ifdef LEVELCOUNT
		setup_levmul(repkc, repseq);
	#endif
	// *** lesson 12
	// if slow is 0 we use the slowest search
	if (slow == 0)
		slowsearch1(repkc, reppc);
	// *** lesson 20
	for (int i = 0; i < FACT8; ++i)
	if (bitp1[i] == 0)
		bitp1[i] = getclearedpage();
	uniq = 0;
	#ifdef FASTCLEAN
		memset(touched, 0, sizeof(touched));
		did_a_prepass = 0;
	#endif
	#ifdef LEVELCOUNTS
		uniq_ulev = 0;
	#endif
	// lesson 26
	// lesson 71
	// lesson 74
}

int main (int argc, char *argv[])
{
	double progstart = waltime();
	duration();
	// PARSE ARGUMENT *** lesson 3
	int oargc = argc;
	char **oargv = argv;
	while (argc > 1 && argv[1][0] == '-')
	{
		--argc;
		++argv;
		switch (argv[0][1])
		{
			// verbose
			case 'v': 
				++verbose;
				break;
			// quiet
			case 'q':
				verbose = 0;
				break;
			// thread
			case 't':
				if (argc < 2)
					error("! not enough arguments to -t");
				if (sscanf(argv[1], "%d", &numthreads) != 1)
					error("! bad thread count arguments");
				if (numthreads < 1 || numthreads > MAX_THREAD)
					error("! bad value for thread count");
				--argc;
				++argv;
				break;
			// *** lesson 5
			// to avoid writing the pruning table
			case 'W':
				++skipwrite;
				break;
			// *** lesson 11
			// setting slowness
			case 's':
				if (argc < 2)
					error("! not enough arguments to -s");
				if (sscanf(argv[1], "%d", &slow) != 1)
					error("! bad -s arguments");
				if (slow < 0 || slow > 2)
					error("! bad value for -s");
				--argc;
				++argv;
				break;
			// setting maxdepth
			case 'd':
				if (argc < 2)
					error("! not enough arguments to -d");
				if (sscanf(argv[1], "%d", &maxdepth) != 1)
					error("! bad -d arguments");
				if (maxdepth < 0)
					error("! bad value for -d");
				if (maxdepth < maxsearchdepht)
					maxsearchdepht = maxdepth;
				--argc;
				++argv;
				break;
			// setting maxsearchdepth
			case 'S':
				if (argc < 2)
					error("! not enough arguments to -S");
				if (sscanf(argv[1], "%d", &maxsearchdepht) != 1)
					error("! bad -S arguments");
				if (maxsearchdepht < 0)
					error("! bad value for -S");
				--argc;
				++argv;
				break;
			// lesson 30
			// lesson 53
			// lesson 67
			// lesson 77
			default:
				error("! bad argument");
		}
	}
	// INITIALIZE THE PROGRAM *** lesson 7
	// we print the arguments and banner
	if (verbose)
		std::cout << BANNER << std::endl << std::flush;
	for (int i = 0; i < oargc; ++i)
		std::cout << " " << oargv[i];
	std::cout << std::endl;
	// lesson 16
	// we allocate the bitmaps
	bitp1 = (unsigned char **)calloc(FACT8, sizeof(unsigned char *));
	bitp2 = (unsigned char **)calloc(FACT8, sizeof(unsigned char *));
	if (bitp1 == 0 || bitp2 == 0)
		error("! no memory");
	// lesson 22
	// lesson 31
	// lesson 33
	// lesson 34
	// lesson 36
	// lesson 40
	// lesson 47
	// lesson 50
	// lesson 84
	// handle the work lesson 81
	// cleanup lesson 54
	phase1::check_integrity();
	std::cout << "Completed in " << (waltime() - progstart) << std::endl;
}