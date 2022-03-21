// *** lesson 2

#include "phase1.hpp"
#include <iostream>
#include <cstdio>

// DATA INSTANTIATION *** lesson 4
unsigned int phase1::memsize;
unsigned char *phase1::mem;
int phase1::file_checksum;
const char *const phase1::filename = "p1p1h.dat";

// lesson 18
static unsigned char map_phase1_offset[KOCSYMM][3];
static int map_phase1[2][12][256];

// UTILITY FUNCTION *** lesson 5
static int datahash(unsigned int *dat, int sz, int seed)
{
	while (sz > 0)
	{
		sz -= 4;
		seed = 37 * seed + *dat++;
	}
	return (seed);
}

// METHOD BODIES *** lesson 9
void phase1::gen_table()
{
	memset(mem, -1, memsize);
	mem[0] = 0;
	int seen = 1;
	std::cout << "Gen phase1" << std::flush;
	for (int d = 1; ; ++d)
	{
		int lastiter = (seen == CORNERRSYMM * EDGEOSYMM * EDGEPERM);
		int seek = d - 1;
		int at = 0;
		for (int cs = 0;  cs < CORNERRSYMM; ++cs)
		{
			int csymm = kocsymm::cornesymm_expand[cs];
			for (int eosymm = 0; eosymm < EDGEOSYMM; ++eosymm)
				for (int epsymm = 0; epsymm < EDGEPERM; ++epsymm, at += BYTES_PER_ENTRY)
					if (mem[at] == seek)
					{
						// HANDLE ONE POSITION *** lesson 10
						int deltadis[NMOVES];
						for (int mv = 0; mv < NMOVES; ++mv)
						{
							int rd = 0;
							kocsymm kc(csymm, eosymm, epsymm);
							kc.move(mv);
							corner_mapinfo & cm = kocsymm::cornersymm[kc.csymm];
							for (int m = cm.minmap; cm.minbits >> m; ++m)
								if ((cm.minbits >> m) & 1)
								{
									int deosymm = kocsymm::edgeomap[kocsymm::edgepxor[kc.epsymm][m >> 3] ^ kc.eosymm][m];
									int depsymm = kocsymm::edgepmap[kc.epsymm][m];
									int dat = ((cm.csymm * EDGEOSYMM + deosymm) * EDGEPERM + depsymm) * BYTES_PER_ENTRY;
									rd = mem[dat];
									if (rd == 255)
									{
										rd = d;
										mem[dat] =  rd;
										++seen;
									}
								}
							deltadis[mv] = rd - seek;
						}
						// ENCODE DELTADIS *** lesson 11
						for (int b = 0; b < 3; ++b)
						{
							int v = 0;
							int clim = 1;
							for (int c = clim; c >= 0; --c)
							{
								int vv = 0;
								int cnts[3];
								cnts[0] = cnts[1] = cnts[2] = 0;
								for (int t = 2; t >= 0; --t)
								{
									vv = 2 * vv + deltadis[3 * b + 9 * c + t];
									cnts[1 + deltadis[3 * b + 9 * c + t]]++;
								}
								if (cnts[0] > 0 && cnts[2] > 0)
									error("! bad delta distance values within one turn set");
								if (cnts[0])
									vv += 7;
								else
									vv += 8;
								v = 16 * v + vv;
							}
							mem[at + b + 1] = v;
						}
					}
		}
		std::cout << " " << d << std::flush;
		if (lastiter)
			break;
	}
	std::cout << " done." << std::endl << std::flush;
}

// lesson 12
const int CHUNKSIZE = 65536;
int phase1::read_table()
{
	FILE *f = fopen(filename, "rb");
	if (f == 0)
		return 0;
	int togo = memsize;
	unsigned char *p = mem;
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
		seed = datahash((unsigned int *)p, siz, seed);
		togo -= siz;
		p += siz;
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

// *** lesson 13
void phase1::write_table()
{
	FILE *f = fopen(filename, "wb");
	if (f == 0)
		error("! cannot write priuning file to current directory");
	if (fwrite(mem, 1, memsize, f) != memsize)
		error("! error writing pruning table");
	if (fwrite(&file_checksum, sizeof(int), 1, f) != 1)
		error("! error writing pruning table");
	fclose(f);
}

// *** lesson 14
void phase1::check_integrity()
{
	if (file_checksum != datahash((unsigned int *)mem, memsize, 0))
		error("! integrity of prning table compromised");
	std::cout << "Verified integrity of phase one pruning data: " << file_checksum << std::endl;
}

// *** lesson 17
int phase1::lookup(const kocsymm & kc)
{
	corner_mapinfo &cm = kocsymm::cornersymm[kc.csymm];
	int m = cm.minmap;
	int r = mem[BYTES_PER_ENTRY * (((cm.csymm * EDGEOSYMM) + kocsymm::edgeomap[kocsymm::edgepxor[kc.epsymm][m >> 3] ^ kc.eosymm][m]) * 495 + kocsymm::edgepmap[kc.epsymm][m])];
	return (r);
}

// *** lesson 19
int phase1::lookup(const kocsymm &kc, int togo, int &nextmovemask)
{
	corner_mapinfo &cm = kocsymm::cornersymm[kc.csymm];
	int m = cm.minmap;
	int off = BYTES_PER_ENTRY * (((cm.csymm * EDGEOSYMM) + 
		kocsymm::edgeomap[kocsymm::edgepxor[kc.epsymm][m >> 3] ^ kc.eosymm][m]) * 495 + 
		kocsymm::edgepmap[kc.epsymm][m]);
	int r = mem[off];
	if (togo < r)
		nextmovemask = 0;
	else if (togo > r + 1)
		nextmovemask = ALLMOVEMASK;
	else
	{
		int (*p)[256] = map_phase1[togo - r];
		unsigned char *o = map_phase1_offset[m];
		nextmovemask = p[o[0]][mem[off + 1]] + p[o[1]][mem[off + 2]] + p[o[2]][mem[off + 3]];
	}
	return(r);
}

void phase1::init(int suppress_writing)
{
	static int initialized = 0;
	if (initialized)
		return;
	initialized = 1;
	// calclulate memory size and allocates array *** lesson 7
	memsize = BYTES_PER_ENTRY * CORNERRSYMM * EDGEOSYMM * EDGEPERM;
	mem = (unsigned char *)malloc(memsize);
	if (mem == 0)
		error("! no memory");
	// *** lesson 15
	if (read_table() == 0)
	{
		gen_table();
		file_checksum = datahash((unsigned int *)mem, memsize, 0);
		if (!suppress_writing)
			write_table();
	}
	// *** lesson 20
	for (int m = 0; m < KOCSYMM; ++m)
	{
		for (int f = 0; f < 3; ++f)
		{
			int mv = f * TWIST;
			int mv2 = cubepos::move_map[m][mv];
			int f2 = mv2 / TWIST;
			int key = 0;
			if (mv2 % TWIST == TWIST - 1)
				++key;
			if (f2 >= 3)
				key += 2;
			key += 4 * (f2 % 3);
			map_phase1_offset[cubepos::invm[m]][f] = key;
		}
	}
	// *** lesson 21
	for (int slack = 0; slack < 2; ++slack)
	{
		for (int key = 0; key < 12; ++key)
		{
			int nv[16];
			for (int nyb = 0; nyb < 16; ++nyb)
			{
				int bits = 0;
				if (slack && nyb <= 7)
					bits = 7;
				else if (slack == 0 && nyb >= 7)
					bits = 0;
				else
					bits = 7 - (nyb & 7);
				if (key & 1)
					bits = ((bits & 1) << 2) + (bits & 2) + (bits >> 2);
				if (key & 2)
					bits <<= 3 * TWIST;
				bits <<= TWIST * (key >> 2);
				nv[nyb] = bits;
			}
			int *a = map_phase1[slack][key];
			for (int byte = 0; byte < 256; ++byte)
				a[byte] = nv[byte & 15] | (((nv[byte >> 4] << (3 * TWIST)) | (nv[byte >> 4] >> (3 * TWIST))) & 0x777777);
		}
	}
}

// *** lesson 22
moveseq phase1::solve(kocsymm kc)
{
	moveseq r;
	int d = phase1::lookup(kc);
	while (d > 0)
	{
		int nmm = 0;
		int t = phase1::lookup(kc, d, nmm);
		if (t == 0)
			break;
		if (t != d)
			error("! did not make progress");
		if (nmm == 0)
			error("! no solutio?");
		int mv = ffs(nmm) - 1;
		r.push_back(mv);
		kc.move(mv);
		d--; 
	}
	return (r);
}