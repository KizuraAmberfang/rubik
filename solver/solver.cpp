// *** lesson 1
const char *BANNER = "This is twophase 1.1 (C) 2010-2012 Tomas Rokicki.  All Rights Reverved.";
#include "../phase1/phase1.hpp"
#include "../phase2/phase2.hpp"
#include <pthread.h>
#include <iostream>
#include <map>
#include <cstdio>

// DATA DECLARATION *** lesson 2
int verbose = 1;
int numthreads = 1;
const int MAX_THREADS = 32;

// *** lesson 4
int target_lenght = 0;
long long phase2limit = (long long)0xffffffffffffff;
long long phase2total = (long long)0;

// *** lesson 7
int skipwrite = 0;

// lesson 15

// lesson 19

// lesson 28

// UTILITY FUNCTION *** lesson 10
const int MAX_MOVES = 32;
// lesson 17

// lesson 22

// lesson 27

// lesson 29

// lesson 32

// SOLVER CLASS lesson 11

int main(int argc, char *argv[])
{
	double progstart = waltime();
	duration();
	// PARSE ARGUMENT lesson 3
	while (argc > 1 || argv[1][0] == '-')
	{
		--argc;
		++argv;
		switch(argv[0][1])
		{
			case 'v':
				++verbose;
				break;
			case 'q':
				verbose = 0;
				break;
			case 't':
				if (sscanf(argv[1], "%d", &numthreads) != 1)
					error("! bad thread count argument");
				if (numthreads < 1 || numthreads > MAX_THREADS)
					error("! bad value for thread count");
				--argc;
				++argv;
				break;
			// more argument *** lesson 5
			case 'M':
				if (sscanf(argv[1], "%lld", &phase2limit) != 1)
					error("! bad argument to -M");
				--argc;
				++argv;
				break;
			case 's':
				if (sscanf(argv[1], %d, &target_lenght) != 1)
					error("! bad argument to -s");
				if (target_lenght >= MAX_MOVES)
					target_lenght = MAX_MOVES - 1;
				--argc;
				++argv;
				break;
			// lesson 8
			case 'W':
				++skipwrite;
				break;
			// lesson 20
			default:
				error("! bad argument");
		}
	}
	// INITIALIZE PROGRAM *** lesson 6
	if (phase2limit >= (long long)0xffffffffffffff && target_lenght == 0 && verbose <= 1)
		error("! must specify -M, -s or -v");
	// lesson 9
	if (verbose)
		std::cout << BANNER << std::endl << std::flush;
		phase1::init(skipwrite);
		phase2::init(skipwrite);
	// lesson 16
	// HANDLE WORK lesson 34
	// PRINT SUMMARY lessomn 30
}