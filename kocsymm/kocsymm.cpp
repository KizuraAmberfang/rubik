// *** lesson 7
#include "kocsymm.hpp"
#include <iostream>

// STATIC DATA INSTANTIATION *** lesson 8

lookup_type kocsymm::cornermove[CORNERSYMM][NMOVES];
lookup_type kocsymm::edgeomove[EDGEOSYMM][NMOVES];
lookup_type kocsymm::edgepmove[EDGEPERM][NMOVES];

// *** lesson 12
lookup_type kocsymm::epsymm_compress[1 << 12];
lookup_type kocsymm::epsymm_expand[EDGEOSYMM];

// *** lesson 21

lookup_type kocsymm::cornesymm_expand[CORNERRSYMM];
corner_mapinfo kocsymm::cornersymm[CORNERSYMM];
lookup_type kocsymm::edgeomap[EDGEOSYMM][KOCSYMM];
lookup_type kocsymm::edgepmap[EDGEPERM][KOCSYMM];
lookup_type kocsymm::edgepxor[EDGEPERM][2];

// *** lesson 32

unsigned char permcube::s4inv[FACT4];
unsigned char permcube::s4mul[FACT4][FACT4];
unsigned char permcube::s4compress[256];
unsigned char permcube::s4expand[FACT4];

// lesson 37
// lesson 40
// lesson 44

// UTILITY METHODS *** lesson 13

static int bc(int v)
{
	int r = 0;
	while (v)
	{
		v &= v - 1;
		++r;
	}
	return (r);
}

// *** lesson 35

int muls4(int a, int b)
{
	int r = 3 & (b >> (2 * (a & 3)));
	r += (3 & (b >> (2 * ((a >> 2) & 3)))) << 2;
	r += (3 & (b >> (2 * ((a >> 4) & 3)))) << 4;
	r += (3 & (b >> (2 * ((a >> 6) & 3)))) << 6;
	return (r);
}

// METHOD BODIES *** lesson 15

kocsymm::kocsymm(const cubepos &cp)
{
	int c = 0, eo = 0, ep = 0;
	for (int i = 6; i >= 0; --i)
		c = 3 * c + cubepos::corner_ori(cp.c[i]);
	for (int i = 10; i >= 0; --i)
	{
		eo = 2 * eo + cubepos::edge_ori(cp.e[i]);
		ep = 2 * ep + (cp.e[i] & 8);
	}
	csymm = c;
	eosymm = eo;
	epsymm = epsymm_compress[ep >> 3];

}

// *** lesson 16

void kocsymm::set_coset(cubepos &cp)
{
	int c = csymm, eo = eosymm, ep = epsymm_expand[epsymm];
	int s = 0;
	for (int i = 0; i < 7; ++i)
	{
		int ori = c % 3;
		cp.c[i] = cubepos::corner_val(i, ori);
		s += ori;
		c = c/3;
	}
	cp.c[7] = cubepos::corner_val(7, (8 * 3 - s) % 3);
	s = 0;
	int nextmid = 4;
	int nextud = 0;
	for (int i = 0; i < 12; ++i)
	{
		if (i == 11)
			eo = s;
		int ori = eo & 1;
		if (ep & 1)
			cp.e[i] = cubepos::edge_val(nextmid++, ori);
		else
		{
			cp.e[i] = cubepos::edge_val(nextud++, ori);
			if (nextud == 4)
				nextud = 8;
		}
		s ^= ori;
		eo >>= 1;
		ep >>= 1;
	}
}

// *** lesson 25

void kocsymm::canon_into(kocsymm &kc) const{
	corner_mapinfo &cm = cornersymm[csymm];
	kc.csymm = cornesymm_expand[cm.csymm];
	kc.eosymm = edgeomap[edgepxor[epsymm][cm.minmap >> 3] ^ eosymm][cm.minmap];
	kc.epsymm = edgepmap[epsymm][cm.minmap];
	for (int m = cm.minmap + 1; cm.minbits >> m; ++m)
		if ((cm.minbits >> m) & 1)
		{
			int neo = edgeomap[edgepxor[epsymm][m >> 3] ^ eosymm][m];
			if (neo > kc.eosymm)
				continue;
			int nep = edgepmap[epsymm][m];
			if (neo < kc.eosymm || nep < kc.epsymm)
			{
				kc.eosymm = neo;
				kc.epsymm = nep;
			}
		}
}

// **** lesson 27

int kocsymm::calc_symm() const
{
	int r = 1;
	corner_mapinfo &cm = cornersymm[csymm];
	int teosymm = edgeomap[edgepxor[epsymm][cm.minmap >> 3] ^ eosymm][cm.minmap];
	int tepsymm = edgepmap[epsymm][cm.minmap];
	for (int m = cm.minmap + 1; cm.minbits >> m; ++m)
		if (((cm.minbits >> m) & 1) && edgeomap[edgepxor[epsymm][m >> 3] ^ eosymm][m] == teosymm && edgepmap[epsymm][m] == tepsymm)
			++r;
	return (r);
}

// *** lesson 33

void permcube::init()
{
	// *** lesson 34
	int cc = 0;
	for (int a = 0; a < 4; ++a)
		for (int b = 0; b < 4; ++b)
			if (a != b)
				for (int c = 0; c < 4; ++c)
				if (a != c || b != c)
				{
					int d = 0 + 1 + 2 + 3 - a - b - c;
					int coor = cc ^ ((cc >> 1) & 1);
					int expanded = (1 << (2 * b)) + (2 << (2 * c)) + (3 << (2 * d));
					s4compress[expanded] = coor;
					s4expand[coor] = expanded;
					++cc;
				}
	for (int i = 0; i < FACT4; ++i)
		for (int j = 0; j < FACT4; ++j)
		{
			int k = s4compress[muls4(s4expand[i], s4expand[j])];
			s4mul[j][i] = k;
			if (k == 0)
				s4inv[i] = j;
		}
	// lesson 38
	// lesson 41
	// lesson 53
	// lesson 54
}

// lesson 46
// lesson 48
// lesson 49
// lesson 50
// lesson 51
// lesson 52

void kocsymm::init()
{
	static int initialized = 0;
	if (initialized)
		return;
	initialized = 1;
	// *** lesson 14
	int c = 0;
	for (int i = 0; i < (1 << 12); ++i)
		if (bc(i) == 4)
		{
			int rotval = ((i << 4) + (i >> 8)) & 0xfff;
			epsymm_compress[rotval] = c;
			epsymm_compress[rotval & 0x7ff] = c;
			epsymm_expand[c] = rotval;
			++c;
		}
	// *** lesson 17
	cubepos cp, cp2;
	for (int i = 0; i < CORNERSYMM; ++i)
	{
		kocsymm kc(i, i % EDGEOSYMM, i % EDGEPERM);
		kc.set_coset(cp);
		for (int mv = 0; mv < NMOVES; ++mv)
		{
			cp2 = cp;
			cp2.movepc(mv);
			kocsymm kc2(cp2);
			cornermove[i][mv] = kc2.csymm;
			if (i < EDGEOSYMM)
				edgeomove[i][mv] = kc2.eosymm;
			if (i < EDGEPERM)
				edgepmove[i][mv] = kc2.epsymm;
		}
	}
	// *** lesson 22
	c = 0;
	for (int cs = 0; cs < CORNERSYMM; ++cs)
	{
		int minval = cs;
		int lowm = 0;
		int lowbits = 1;
		kocsymm kc(cs, 0, 0);
		for (int m = 1; m < KOCSYMM; ++m)
		{
			kc.set_coset(cp);
			cp.remap_into(m, cp2);
			kocsymm kc2(cp2);
			if (kc2.csymm < minval)
			{
				minval = kc2.csymm;
				lowbits = (1 << m);
				lowm = m;
			}
			else if (kc2.csymm == minval)
				lowbits |= (1 << m);
		}
		if (minval == cs)
		{
			cornesymm_expand[c] = minval;
			cornersymm[cs].csymm = c++;
		}
		cornersymm[cs].minbits = lowbits;
		cornersymm[cs].minmap = lowm;
		cornersymm[cs].csymm = cornersymm[minval].csymm;
	}
	if (c != CORNERRSYMM)
		error("! bad cornersym result");
	// *** lesson 23
	for (int ep = 0; ep < EDGEPERM; ++ep)
	{
		kocsymm kc(0, 0, ep);
		for (int m = 0; m < KOCSYMM; ++m)
		{
			kc.set_coset(cp);
			cp.remap_into(m, cp2);
			kocsymm kc2(cp2);
			edgepmap[ep][m] = kc2.epsymm;
			if (m == 8)
			{
				edgepxor[kc2.epsymm][0] = 0;
				edgepxor[kc2.epsymm][1] = kc2.eosymm;
			}
		}
	}
	for (int eo = 0; eo < EDGEOSYMM; ++eo)
	{
		kocsymm kc(0, eo, 0);
		for (int m = 0; m < KOCSYMM; ++m)
		{
			kc.set_coset(cp);
			cp.remap_into(m, cp2);
			kocsymm kc2(cp2);
			edgeomap[eo][m] = kc2.eosymm;
		}
	}
	permcube::init();
}