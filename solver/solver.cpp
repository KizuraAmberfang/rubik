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
pthread_mutex_t my_mutex;

// lesson 19

// lesson 28

// UTILITY FUNCTION *** lesson 10
const int MAX_MOVES = 32;

// lesson 17
void get_global_lock()
{
	pthread_mutex_lock(&my_mutex);
}

void release_global_lock()
{
	pthread_mutex_unlock(&my_mutex);
}

// lesson 22

// lesson 27

// lesson 29

// lesson 32

// SOLVER CLASS *** lesson 11
class twophasesolver{
	public:
	twophasesolver(){}
	cubepos pos;
	long long phase2probes;
	int bestsol;
	int finished;
	int curm;
	int solmap;
	int seq;

	// SOLVER DATA *** lesson 12
	unsigned char moves[MAX_MOVES];
	unsigned char bestmoves[MAX_MOVES];

	// lesson 18
	kocsymm kc6[6], kccanon6[6];
	cubepos cp6[6];
	permcube pc6[6];
	int mindepth[6];
	char uniq[6];
	int minmindepth;

	// SOLVER METHODS *** lesson 13
	void solve(int seqarg, cubepos &cp)
	{
		// INITIALIZE THE SOLVER *** lesson 14
		pos = cp;
		phase2probes = 0;
		bestsol = MAX_MOVES;
		finished = 0;
		seq = seqarg;
		// SET UP POSITION INVARIANTS lesson 21
		// SOLVE ONE POSITION lesson 23
		// CHECK AND REPORT SOLUTION lesson 26
	}
	
	// lesson 24
	
	// lesson 25
	
	// lesson 31

	// lesson 33

	char pad[256];
} solvers[MAX_THREADS];

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
	pthread_mutex_init(&my_mutex, NULL); // check!
	// HANDLE WORK lesson 34
	// PRINT SUMMARY lessomn 30
}