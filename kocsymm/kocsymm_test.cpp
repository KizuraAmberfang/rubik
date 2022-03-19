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

	// TEST CANONICALIZATION lesson 59

	// COUNT COSETS lesson 60

	// MOVE TIMING TESTS lesson 61
	return 0;
}