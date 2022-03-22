#include "phase2.hpp"
#include <iostream>

char buf[4096];

int main(int atgc, char *argv[])
{
	if (lrand48() == 0)
		srand48(time(0));
	phase2::init(0);
	phase2::check_integrity();
	cubepos cp;
	for (int i = 0; i < 100000; ++i)
	{
		char *tmp;
		int mv = random_move();
		if (kocsymm::in_Kocieba_group(mv))
			cp.movepc(mv);
		int lookd = phase2::lookup(cp);
		std::cout << "Distance " << lookd << std::endl;
		moveseq s = phase2::solve(cp);
		cubepos cpt = cp;
		for (unsigned int j = 0; j < s.size(); ++j)
			cpt.movepc(s[j]);
		cubepos::append_moveseq(tmp = buf, s);
		std::cout << "Solution lenght " << s.size() << " " << buf << std::endl;
		if (cpt != identity_cube)
			error("! bad solve");
		if ((unsigned int) lookd > s.size())
			error("! solution too short");
	}
}