// *** lesson 7
#include "kocsymm.hpp"
#include <iostream>

// STATIC DATA INSTANTIATION *** lesson 8

lookup_type kocsymm::cornermove[CORNERSYMM][NMOVES];
lookup_type kocsymm::edgeomove[EDGEOSYMM][NMOVES];
lookup_type kocsymm::edgepmove[EDGEPERM][NMOVES];

// lesson 12
lookup_type epsymm_compress[1 << 12];
lookup_type epsymm_expand[EDGEOSYMM];

// lesson 21
// lesson 32
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
// lesson 35

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
	cp.c[7] = cubepos::edge_val(7, (8 * 3 - s) % 3);
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

// lesson 25
// lesson 27
// lesson 33
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
	// lesson 22
	// lesson 23

	//permcube::init();
}