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

// lesson 4

// lesson 7

// lesson 15

// lesson 19

// lesson 28

// UTILITY FUNCTION lesson 10

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
			// more argument lesson 5
			default:
				error("! bad argument");
		}
	}
	// INITIALIZE PROGRAM lesson 6
	// HANDLE WORK lesson 34
	// PRINT SUMMARY lessomn 30
}