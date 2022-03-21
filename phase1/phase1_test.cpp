#include "phase1.hpp"
#include <iostream>
int main(int argc, char *argv[])
{
	if (lrand48() == 0)
		srand48(time(0));
	phase1::init();
	int t[10000];
	for (int i = 0; i < 10000; ++i)
		t[i] = random_move();
	kocsymm kc;
	// CHECK NEXT MOVE INFO lesson 24
	for (int i = 0; i < 10000; ++i)
	{
		kc.move(t[i]);
		int d = phase1::lookup(kc);
		for (int d2 = d - 1; d2 <= d + 2; ++d2)
		{
			int nextmovemask = 0;
			kocsymm kc2;
			int dt = phase1::lookup(kc, d2, nextmovemask);
			if (dt != d)
				error("mismatch on lookup");
			int bc = 0;
			for (int mv = 0; mv < NMOVES; ++mv)
			{
				kc2 = kc;
				kc2.move(mv);
				dt = phase1::lookup(kc2);
				if (dt < d2)
					bc |= (1 << mv);
			}
			if (nextmovemask != bc)
				error("! move mask error");
		}
	}
	// CHECK BASIC LOOKUP PERFORMANCE lesson 25
	int sum = 0;
	duration();
	for (int i = 0; i < 10000; ++i)
		for (int j = 0; j < 10000; ++j)
		{
			kc.move(t[j]);
			sum += phase1::lookup(kc);
		}
	std::cout << "Did 100M basic lookups in " << duration() << " sum " << sum << std::endl;
	// CHECK EXTENDEND LOOKUP PERFORMANCE lesson 26
	int prev = 10;
	int nextmovemask = 0;
	for (int i = 0; i < 10000; ++i)
		for (int j = 0; j < 10000; ++j)
		{
			kc.move(t[j]);
			int r = phase1::lookup(kc, prev, nextmovemask);
			sum += r + nextmovemask;
			prev = r;
		}
	std::cout << "Did 100M extended lookups in " << duration() << " sum " << sum << std::endl;
	// CHECK SOLUTION PERFORMANCA lesson 27
	for (int i = 0; i < 1000; ++i)
		for (int j = 0; j < 10000; ++j)
		{
			kc.move(t[j]);
			phase1::solve(kc);
		}
	std::cout << "Did 10M solves in " << duration() << " sum " << sum << std::endl;
}