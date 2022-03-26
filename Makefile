all:
	@echo "Let's solve a Rubik Cube!"
	g++ solver/solver.cpp phase1/phase1.cpp phase2/phase2.cpp kocsymm/kocsymm.cpp cubepos/cubepos.cpp -o solve

cube:
	g++ cubepos/*.cpp -o cubepos_test
	./cubepos_test

koc:
	g++ kocsymm/*.cpp cubepos/cubepos.cpp -o kocsymm_test
	./kocsymm_test

ph1:
	g++ phase1/*.cpp kocsymm/kocsymm.cpp cubepos/cubepos.cpp -o phase1_test
	./phase1_test

ph2:
	g++ phase2/*.cpp kocsymm/kocsymm.cpp cubepos/cubepos.cpp -o phase2_test
	./phase2_test
