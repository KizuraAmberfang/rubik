all:
	@echo "Let's solve a Rubik Cube!"

cube:
	g++ cubepos/*.cpp -o cubepos_test
	./cubepos_test

koc:
	g++ kocsymm/*.cpp cubepos/cubepos.cpp -o kocsymm_test
	./kocsymm_test

ph1:
	g++ phase1/*.cpp kocsymm/kocsymm.cpp cubepos/cubepos.cpp -o phase1_test
	./phase1_test