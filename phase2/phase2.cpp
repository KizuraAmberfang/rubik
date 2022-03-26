// *** lesson 3

#include "phase2.hpp"
#include <iostream>
#include <cstdio>

// DATA INSTANTIATION *** lesson 4
struct corner_reduce
{
	unsigned char m;
	unsigned char parity;
	lookup_type c;
	lookup_type minbits;
}	corner_reduction[FACT8];

lookup_type edgeud_remap[KOCSYMM][FACT8];

// *** lesson 7
int phase2::cornermax;
unsigned int phase2::memsize;
unsigned int *phase2::mem;

// *** lesson 18

const char *const phase2::filename = "p2p1h.dat";
int phase2::file_checksum;

// UTILITY METHOD *** lesson 5
inline int corner_coordinate(const permcube &pc)
{
	return ((pc.c8_4 * FACT4 + pc.ctp) * FACT4 + pc.cbp);
}
inline int edge_coordinate(const permcube & pc)
{
	return ((permcube::c12_8[pc.et] * FACT4 + pc.etp) * FACT4 + pc.ebp);
}

// *** lesson 19 
static int datahash(unsigned int *dat, int sx, int seed)
{
	while (sx > 0)
	{
		sx -= 4;
		seed = 37 * seed + *dat++;
	}
	return (seed);
}

// METHOD BODIES *** lesson 11

int phase2::lookup(const cubepos &cp)
{
	permcube pc(cp);
	return (lookup(pc));
}

int phase2::lookup(const permcube &pc)
{
	int cc = corner_coordinate(pc);
	corner_reduce &cr = corner_reduction[cc];
	int off = cr.c * FACT8 + edgeud_remap[cr.m][edge_coordinate(pc)];
	int r = (mem[off >> 3] >> (4 * (off & 7))) & 0xf; // verifica
	if (r == 0 && pc == identity_pc)
		return (0);
	else
		return (r + 1);
}

// *** lesson 13

void phase2::gen_table()
{
	memset(mem, 255, memsize);
	std::cout << "Gen phase2" << std::flush;
	mem[0] &= ~14; // verifica
	int seen = 1;
	for (int d = 0; d < 15; ++d)
	{
		unsigned int seek = (d ? d - 1 : 1);
		int newval = d;
		for (int c8_4 = 0; c8_4 < C8_4; ++c8_4)
			for (int ctp = 0; ctp < FACT4; ++ctp)
				for (int cbp = 0; cbp < FACT4; ++cbp)
				{
					permcube pc;
					pc.c8_4 = c8_4;
					pc.ctp = ctp;
					pc.cbp = cbp;
					int oc = corner_coordinate(pc);
					corner_reduce &cr = corner_reduction[oc];
					if (cr.minbits & 1)
					{
						// Iterate over all moves *** lesson 14
						permcube pc2, pc3, pc4;
						cubepos cp2, cp3;
						int off = corner_reduction[oc].c * (FACT8 / 8);
						for (int mv = 0; mv < NMOVES; ++mv)
						{
							if (!kocsymm::in_Kocieba_group(mv))
								continue;
							pc2 = pc;
							pc2.move(mv);
							int dest_off = corner_coordinate(pc2);
							corner_reduce &cr = corner_reduction[dest_off];
							int destat = cr.c * (FACT8 / 8);
							for (int m = cr.m; (1 << m) <= cr.minbits; ++m)
								if ((cr.minbits >> m) & 1)
								{
									// scan one row *** lesson 15
									int at = 0;
									for (int e8_4 = 0; e8_4 < C8_4; ++e8_4)
									{
										int et = permcube::c8_12[e8_4];
										int t1 = permcube::eperm_move[et][mv];
										int eb = kocsymm::epsymm_compress[0xf0f - kocsymm::epsymm_expand[et]];
										int t2 = permcube::eperm_move[eb][mv] & 31;
										int dst1 = permcube::c12_8[t1 >> 5] * 24 * 24;
										t1 &= 31;
										for (int etp = 0; etp < FACT4; ++etp)
											for (int ebp = 0; ebp < FACT4; ++ebp, ++at)
											{
												if (mem[off + (at >> 3)] == 0xffffffff)
												{
													ebp += 7;
													at += 7;
												}
												else if (((mem[off + (at >> 3)] >> (4 * (at & 7))) & 0xf) == seek)
												{
													// handle one position *** lesson 16
													int etp1 = permcube::s4mul[etp][t1];
													int ebp1 = permcube::s4mul[ebp][t2];
													int dat = edgeud_remap[m][dst1 + etp1 * 24 + ebp1];
													int val = (mem[destat + (dat >> 3)] >> (4 * (dat & 7))) & 0xf;
													if (val == 0xf)
													{
														mem[destat + (dat >> 3)] -= (0xf - newval) << (4 * (dat & 7));
														++seen;
													}
												}
											}
									}
								}
						}
					}
				}
		#ifndef QUARTER
			if (d == 0)
				mem[0] &= ~15;
		#endif
			std::cout << " " << d << std::flush;
	}
	std::cout << " done." << std::endl << std::flush;
}

// *** lesson 20
const int CHUNKSIZE = 65536;
int phase2::read_table()
{
	FILE *f = fopen(filename, "rb");
	if (f == 0)
		return (0);
	int togo = memsize;
	unsigned int *p = mem;
	int seed = 0;
	while (togo > 0)
	{
		unsigned int siz = (togo > CHUNKSIZE ? CHUNKSIZE : togo);
		if (fread(p, 1, siz, f) != siz)
		{
			std::cerr << "Out of data in " << filename << std::endl;
			fclose(f);
			return (0);
		}
		seed = datahash(p, siz, seed);
		togo -= siz;
		p += siz / sizeof(unsigned int);
	}
	if (fread(&file_checksum, sizeof(int), 1, f) != 1)
	{
		std::cerr << "Out of data in " << filename << std::endl;
		fclose(f);
		return (0);
	}
	fclose(f);
	if (file_checksum != seed)
	{
		std::cerr << "Bad checksum in " << filename << "; expected " << file_checksum << " but saw " << seed << std::endl;
		return (0);
	}
	return (1);
}

// lesson 21

void phase2::write_table()
{
	FILE *f = fopen(filename, "wb");
	if (f == 0)
		error("! cannot write pruning file to current directory");
	if (fwrite(mem, 1, memsize, f) != memsize)
		error("! error writing pruning table");
	if (fwrite(&file_checksum, sizeof(int), 1, f) != 1)
		error("! error writing pruning table");
	fclose(f);
}

// lesson 22

void phase2::check_integrity()
{
	if (file_checksum != datahash(mem, memsize, 0))
		error("! integrity of pruning table compromised");
	std::cout << "Verified integrity of phase two pruning data: " << file_checksum << std::endl;
}

// lesson 25

moveseq phase2::solve(const permcube &pc, int maxlen)
{
	moveseq r;
	for (int d = lookup(pc); d <= maxlen; ++d)
		if (solve(pc, d, CANONSEQSTART, r))
		{
			reverse(r.begin(), r.end());
			break;
		}
	return (r);
}

int phase2::solve(const permcube &pc, int togo, int canonstate, moveseq &r)
{
	if (lookup(pc) > togo)
		return (0);
	if (pc == identity_pc)
		return (1);
	if (togo-- <= 0)
		return (0);
	permcube pc2;
	int mask = cubepos::cs_mask(canonstate) & 0x25d2e97; // verifica
	while (mask)
	{
		int ntogo = togo;
		int mv = ffs(mask) - 1;
		mask &= mask - 1;
		pc2 = pc;
		pc2.move(mv);
		if (solve(pc2, ntogo, cubepos::next_cs(canonstate, mv), r))
		{
			r.push_back(mv);
			return (1);
		}
	}
	return (0);
}

void phase2::init(int suppress_writing)
{
	static int initialized = 0;
	if (initialized) 
		return;
	// *** lesson 8
	cornermax = 0;
	for (int c8_4 = 0; c8_4 < C8_4; ++c8_4)
		for (int ctp = 0; ctp < FACT4; ++ctp)
			for (int cbp = 0; cbp < FACT4; ++cbp)
			{
				permcube pc;
				pc.c8_4 = c8_4;
				pc.ctp = ctp;
				pc.cbp = cbp;
				int oc = corner_coordinate(pc);
				int minc = oc;
				int minm = 0;
				int minbits = 1;
				cubepos cp;
				pc.set_perm(cp);
				for (int m = 1; m < 16; ++m)
				{
					cubepos cp2;
					cp.remap_into(m, cp2);
					permcube pc2(cp2);
					int tc = corner_coordinate(pc2);
					if (tc < minc)
					{
						minc = tc;
						minm = m;
						minbits = 1 << m;
					}
					else if (tc == minc)
						minbits |= (1 << m);
				}
				corner_reduce &cr = corner_reduction[oc];
				if (oc == minc)
					cr.c = cornermax++;
				cr.m = minm;
				cr.c = corner_reduction[minc].c;
				cr.minbits = minbits;
				cr.parity = (permcube::c8_4_parity[c8_4] + ctp + cbp) & 1;
			}
	// *** lesson 9
	int at = 0;
	cubepos cp, cp2;
	for (int e8_4 = 0; e8_4 < C8_4; ++e8_4)
	{
		permcube pc;
		pc.et = permcube::c8_12[e8_4];
		pc.eb = kocsymm::epsymm_compress[0xf0f - kocsymm::epsymm_expand[pc.et]];
		for (int etp = 0; etp < FACT4; ++etp)
		{
			pc.etp = etp;
			for (int ebp = 0; ebp < FACT4; ++ebp, ++at)
			{
				pc.ebp = ebp;
				for (int m = 0; m < KOCSYMM; ++m)
				{
					pc.set_edge_perm(cp);
					cp.remap_into(m, cp2);
					permcube pc2(cp2);
					edgeud_remap[m][at] = edge_coordinate(pc2);
				}
			}
		}
	}
	// *** lesson 10
	memsize = cornermax * (FACT8 / 2);
	mem = (unsigned int *)malloc(memsize);
	if (mem == 0)
		error("! no memory in phase2");
	// *** lesson 23
	if (read_table() == 0)
	{
		gen_table();
		file_checksum = datahash(mem, memsize, 0);
		if (!suppress_writing)
			write_table();
	}
}