#include "kocsymm.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
	if (lrand48() == 0)
		srand48(getpid() + time(0));
	kocsymm kc, kc2;
	permcube pc, pc2;
	cubepos cp, cp2;
	// BASIC TEST *** lesson 56
	{
		std::cout << "Basic Test" << std::endl;
		cubepos cpi;
		permcube pci(cpi);
		kocsymm kci(cpi);
		permcube pct;
		kocsymm kct;
		if (pct != pci || kct != kci)
			error("! problem with default constructors");
		if (permcube::c12_8[pc.et] != 0)
			error("! bad mapping in 12->8");
	}
	// TEST CONVERSIONS BACK AND FORTH *** lesson 57
	std::cout << "Test conversions back and forth" << std::endl;
	for (int i = 0; i < 10000; ++i)
	{
		cp.randomize();
		kocsymm kc(cp);
		permcube pc(cp);
		if (kc.epsymm != pc.em)
			error("! mismatch in edge middle occupancy");
		kc.set_coset(cp2);
		pc.set_perm(cp2);
		if (cp != cp2)
			error("! mismatch in conversion and back");
	}
	// TEST MOVE ROUTINES lesson 58
	std::cout << "Test move routines" << std::endl;
	for (int i = 0; i < 1000; ++i)
	{
		cp.randomize();
		kocsymm kc(cp);
		permcube pc(cp);
		int mv = random_move_ext();
		cp.movepc(mv);
		cp2 = cp;
		kc.move(mv);
		pc.move(mv);
		kc.set_coset(cp2);
		pc.set_perm(cp2);
		if (cp != cp2)
			error("! mismatch in move test");
	}
	// TEST CANONICALIZATION lesson 59
	std::cout << "Test canonicalization" << std::endl;
	for (int i = 0; i < 1000; ++i)
	{
		cp.randomize();
		kocsymm kc(cp);
		for (int m = 1; m < KOCSYMM; ++m)
		{
			cp.remap_into(m, cp2);
			kocsymm kc2(cp2);
			if (kc2 < kc)
				kc = kc2;
		}
		kocsymm kc3(cp);
		kc3.canon_into(kc2);
		if (kc2 != kc)
			error("! canonicalization failure");
	}
	// COUNT COSETS lesson 60
	std::cout << "Test count cosets" << std::endl;
	int s = 0;
	for (int c = 0; c < CORNERSYMM; ++c)
	{
		int bits = kocsymm::cornersymm[c].minbits;
		if (bits == 1)
			s += EDGEOSYMM * EDGEPERM;
		else if (bits & 1)
		{
			for (int eo = 0; eo < EDGEOSYMM; ++eo)
				for (int ep = 0; ep < EDGEPERM; ++ep)
				{
					kocsymm kc(c, eo, ep);
					kc.canon_into(kc2);
					if (kc == kc2)
						++s;
				}
		}
	}
	std::cout << "Final sum is " << s << std::endl;
	if (s != 138639780)
		error("! bad total coset calculation");
	// MOVE TIMING TESTS lesson 61
	std::cout << "Timing test" << std::endl;
	int mvs[10000];
	for (int i = 0; i < 10000; ++i)
		mvs[i] = random_move();
	duration();
	for (int i = 0; i < 10000; ++i)
		for (int j = 0; j < 10000; ++j)
			kc.move(mvs[i]);
	std::cout << "Moving 100M kc took " << duration() << std::endl;
	for (int i = 0; i < 10000; ++i)
		for (int j = 0; j < 10000; ++j)
			pc.move(mvs[i]);
	std::cout << "Moving 100M pc took " << duration() << std::endl;
	for (int i = 0; i < 10000; ++i)
		for (int j = 0; j < 10000; ++j)
			cp.move(mvs[i]);
	std::cout << "Moving 100M cp (move) took " << duration() << std::endl;
	for (int i = 0; i < 10000; ++i)
		for (int j = 0; j < 10000; ++j)
			cp.movepc(mvs[i]);
	std::cout << "Moving 100M cp (movepc) took " << duration() << std::endl;
	if (cp < cp2 && pc < pc2 && kc < kc2)
		std::cout << "(Ignore this message.)" << std::endl;
	return 0;
}