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
// *** lesson 21
// array to traslate a FACT4 value into a particolar offset
unsigned char permtobit[FACT4];
unsigned char bittoperm[FACT4];
// *** lesson 23
// this is for performance
unsigned char saveb;
unsigned char *savep;
// *** lesson 29
int disable_prepass = 0;
// *** lesson 32
const int SQMOVES = 3;
short rearrange[2][SQMOVES][1 << 12];
// *** lesson 35
const int PREPASS_MOVES = 10;
unsigned short eperm_map[FACT8 / 2][PREPASS_MOVES];
// *** lesson 39
#ifdef LEVELCOUNTS
	unsigned char bc[1 << 12];
#endif
// *** lesson 43
const int STRIDE = 16;
// *** lesson 44
#include "corner_order.hpp"
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
// *** lesson 24
void flushbit()
{
	if (savep != 0)
	{
		if (0 == (*savep & saveb))
		{
			*savep |= saveb;
			++uniq;
		}
		savep = 0;
	}
}
void setonebit(const permcube &pc)
{
	int cindex = (pc.c8_4 * FACT4 + pc.ctp) * FACT4 + pc.cbp;
	unsigned int eindex = (((permcube::c12_8[pc.et] * FACT4) + pc.etp) * FACT4 / 2 + (pc.ebp >> 1)) * FACT4 + permtobit[pc.emp];
	#ifdef FASTCLEAN
		touched[cindex] = 1;
	#endif
		++probes;
		flushbit();
		savep = bitp1[cindex] + (eindex >> 3);
		__builtin_prefetch(savep);
		saveb = 1 << (eindex & 7);
}
// *** lesson 25
// second routine, we don't do the last 2 lookups because it's not necessary
void slowsearch2(const kocsymm &kc, const permcube &pc, int togo, int movemask, int canon)
{
	if (togo == 0)
	{
		if (kc == identity_kc)
			setonebit(pc);
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
			int movemask3 = newmovemask & cubepos::cs_mask(new_canon);
			if (togo == 1)
			{
				permcube pc3;
				while (movemask3)
				{
					int mv2 = ffs(movemask3) - 1;
					movemask3 &= movemask3 - 1;
					pc3 = pc2;
					pc3.move(mv2);
					setonebit(pc3);
				}
			}
			else
				slowsearch2(kc2, pc2, togo, movemask3, new_canon);
		}
	}
}

// *** lesson 27
void slowsearch2(const kocsymm &kc, const permcube &pc)
{
	duration();
	for (int d = phase1::lookup(repkc); d < maxsearchdepht; ++d)
	{
		probes = 0;
		long long prevlev = uniq;
		slowsearch2(kc, pc, d, ALLMOVEMASK, CANONSEQSTART);
		flushbit();
		long long thislev = uniq - prevlev;
		if (verbose)
			std::cout << "Tests at "  << d << " " << probes << " in " << duration() << " uniq " << uniq << " lev " << thislev << std::endl;
	}
}

// *** lesson 28
void unpack_edgecoord(permcube &pc, int e8_4, int epp1, int epp2)
{
	pc.et = permcube::c8_12[e8_4];
	pc.etp = epp1;
	pc.ebp = epp2;
	pc.eb = kocsymm::epsymm_compress[0xf0f - kocsymm::epsymm_expand[pc.et]];
	pc.em = 0;
}

void unpack_edgecoord(permcube &pc, int coord)
{
	unpack_edgecoord(pc, coord/(FACT4 * FACT4), coord / FACT4 % FACT4, coord % FACT4);
}

// *** lesson 37
void innerloop3(unsigned char *dst, unsigned char **srcs, int base)
{
	dst += 3 * base;
	unsigned char *end = dst + 12 * 24 * 3;
	unsigned char tval = *end;
	unsigned short *cpp = eperm_map[base];
	for (; dst < end; dst += 3, cpp += PREPASS_MOVES)
	{
		int wf2 = *(int *)(srcs[3] + cpp[3]);
		int wr2 = *(int *)(srcs[4] + cpp[4]);
		int wb2 = *(int *)(srcs[8] + cpp[8]);
		int wl2 = *(int *)(srcs[9] + cpp[9]);
		*(int *)dst = (((wf2 & 0xfff) | 
			rearrange[0][0][wr2 & 0xfff] |
			rearrange[0][1][wb2 & 0xfff] |
			rearrange[0][2][wl2 & 0xfff]) << 12) |
			((wf2 >> 12) & 0xfff) | 
			rearrange[1][0][(wr2 >> 12) & 0xfff] |
			rearrange[0][1][(wb2 >> 12) & 0xfff] |
			rearrange[1][2][(wl2 >> 12) & 0xfff] |
			*(int *)(srcs[0] + cpp[0]) |
			*(int *)(srcs[1] + cpp[1]) |
			*(int *)(srcs[2] + cpp[2]) |
			*(int *)(srcs[5] + cpp[5]) |
			*(int *)(srcs[6] + cpp[6]) |
			*(int *)(srcs[7] + cpp[7]);
	}
	*end = tval;
}
// *** lesson 38
int countbit(unsigned int *a)
{
	int r = 0;
	const unsigned int mask1 = 0x55555555;
	const unsigned int mask2 = 0x33333333;
	const unsigned int mask3 = 0x0f0f0f0f;
	for (int i = 0; i < PAGESIZE; i += 24)
	{
		unsigned int w1 = *a++;
		unsigned int w2 = *a++;
		unsigned int w3 = *a++;
		w1 = (w1 & mask1) + ((w1 >> 1) & mask1) + (w2 & mask1);
		w2 = ((w2 >> 1) & mask1) + (w3 & mask1) + ((w3 >> 1) & mask1);
		unsigned int s1 = (w1 & mask2) + ((w1 >> 2) & mask2) + (w2 & mask2) + ((w2 >> 2) & mask2);
		s1 = (s1 & mask3) + ((s1 >> 4) & mask3);
		w1 = *a++;
		w2 = *a++;
		w3 = *a++;
		w1 = (w1 & mask1) + ((w1 >> 1) & mask1) + (w2 & mask1);
		w2 = ((w2 >> 1) & mask1) + (w3 & mask1) + ((w3 >> 1) & mask1);
		unsigned int s2 = (w1 & mask2) + ((w1 >> 2) & mask2) + (w2 & mask2) + ((w2 >> 2) & mask2);
		s1 += (s2 & mask3) + ((s2 >> 4) & mask3);
		r += 255 & ((s1 >> 24) + (s1 >> 16) + (s1 >> 8) + s1);
	}
	return (r);
}
// *** lesson 41
#ifdef LEVELCOUNTS
	int parity(int coord)
	{
		return (permcube::c8_4_parity[coord / (FACT4 * FACT4)] ^ (coord / 24) ^ coord) & 1;
	}
	int countbits2(int cperm, int *a, int &rv2)
	{
		int coparity = parity(cperm);
		int r = 0, r2 = 0;
		int ind = 0;
		for (int e8_4 = 0; e8_4 < C8_4; ++e8_4)
		{
			int p2 = coparity ^ permcube::c8_4_parity[e8_4];
			for (int epp1 = 0; epp1 < FACT4; ++epp1)
			{
				int off1 = (p2 ^ epp1) & 1;
				int off2 = 1 - off1;
				for (int epp2 = 0; epp2 < FACT4; epp2 += 2, ind += 2)
				{
					int w = *a;
					int v1 = bc[(w & 0xfff)];
					int v2 = bc[((w >> 12) & 0xfff)];
					r += v1 + v2;
					r2 += v1 * levmul[ind + off1] + v2 * levmul[ind + off2];
					a = (int *)(((char *) a) + 3);
				}
			}
		}
		rv2 = r2;
		return r;
	}
#endif
// *** lesson 42
struct elemdata {
	unsigned char *dst;
	unsigned char *from[PREPASS_MOVES];
	unsigned char e84map[70];
};
// *** lesson 45
void calcneighbors(int cperm, int *a)
{
	permcube pc, pc2;
	pc.c8_4 = cperm / (FACT4 * FACT4);
	pc.ctp = cperm / FACT4 % FACT4;
	pc.cbp = cperm % FACT4;
	for (int mv = 0; mv < NMOVES; ++mv)
	{
		if (!kocsymm::in_Kocieba_group(mv))
			continue;
		pc2 = pc;
		pc2.move(mv);
		*a++ = (pc2.c8_4 * FACT4 + pc2.ctp) * FACT4 + pc2.cbp;
	}
}

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
	// *** lesson 26
	if (slow == 1)
		slowsearch2(repkc, reppc);
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
			// disable prepass
			case 'U':
				++disable_prepass;
				break;
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
	// *** lesson 16
	// we allocate the bitmaps
	bitp1 = (unsigned char **)calloc(FACT8, sizeof(unsigned char *));
	bitp2 = (unsigned char **)calloc(FACT8, sizeof(unsigned char *));
	if (bitp1 == 0 || bitp2 == 0)
		error("! no memory");
	// *** lesson 22
	// initialize in sequential order
	for (int i = 0; i < FACT4; ++i)
	{
		permtobit[i] = i;
		bittoperm[i] = i;
	}
	// *** lesson 31
	// we set for some moves the bits
	const int F2 = 1 + TWIST;
	const int R2 = 1 + 2 * TWIST;
	const int B2 = 1 + 4 * TWIST;
	const int L2 = 1 + 5 * TWIST;
	permcube pc;
	for (int i = 0; i < FACT4 / 2; ++i)
	{
		permtobit[2 * i] = i;
		pc.emp = 2 * i;
		pc.move(F2);
		permtobit[pc.emp] = 12 + i;
	}
	for (int i = 0; i < FACT4; ++i)
		bittoperm[permtobit[i]] = i;
	// *** lesson 33
	const int mvs[] = {R2, B2, L2};
	for (int mvi = 0; mvi < SQMOVES; ++mvi)
		for (int p = 0; p < FACT4; ++p)
		{
			pc.emp = p;
			pc.move(mvs[mvi]);
			rearrange[p & 1][mvi][1 << (permtobit[p] % 12)] = 1 << (permtobit[pc.emp] % 12);
		}
	for (int p = 0; p < 2; ++p)
		for (int mvi = 0; mvi < SQMOVES; ++mvi)
			for (int i = 1; i < (1 << 12); ++i)
			{
				int lowb = i & -1;
				rearrange[p][mvi][i] = rearrange[p][mvi][lowb] | rearrange[p][mvi][i - lowb];
			}
	// *** lesson 34
	for (int i = 0; i < (1 << 12); ++i)
		if (rearrange[0][1][i] != rearrange[1][1][i])
			error("! mismatch in rearrange");
	// *** lesson 36
	int ind = 0;
	for (int e8_4 = 0; e8_4 < C8_4; ++e8_4)
		for (int epp1 = 0; epp1 < FACT4; ++epp1)
			for (int epp2 = 0; epp2 < FACT4; epp2 += 2, ++ind)
			{
				int mvi = 0;
				for (int mv = 0; mv < NMOVES; ++mv)
				{
					if (!kocsymm::in_Kocieba_group(mv))
						continue;
					unpack_edgecoord(pc, e8_4, epp1, epp2);
					pc.move(mv);
					eperm_map[ind][mvi] == permcube::c12_8[pc.et] * FACT4 + pc.etp) * FACT4 + pc.ebp) / 2 * 3;
					++mvi;
				}
			}
	// *** lesson 40
	#ifdef LEVELCOUNTS
		for (int i = 1; i < (1 << 12); ++i)
			bc[i] = 1 + bc[i & (i - 1)];
	#endif
	// lesson 47
	// lesson 50
	// lesson 84
	// handle the work lesson 81
	// cleanup lesson 54
	phase1::check_integrity();
	std::cout << "Completed in " << (waltime() - progstart) << std::endl;
}