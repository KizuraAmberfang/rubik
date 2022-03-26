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

// *** lesson 15
pthread_mutex_t my_mutex;

// *** lesson 19
int axemask = 63;

// *** lesson 28
class solution
{
	public:
	solution(const cubepos &cparg, int seqarg, long long p2parg, moveseq &solarg)
	{
		cp = cparg;
		seq = seqarg;
		phase2probes = p2parg;
		moves = solarg;
	}

	solution() {}

	cubepos cp;
	int seq;
	long long phase2probes;
	moveseq moves;
};
std::map <int, solution> queue;
int next_sequence = 1;
int missed_target = 0;
int solved = 0;

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

// *** lesson 22
int sloweq(const cubepos &cp1, const cubepos &cp2)
{
	cubepos cp3;
	for (int m = 0; m < KOCSYMM; ++m)
	{
		cp2.remap_into(m, cp3);
		if (cp1 == cp3)
			return (1);
	}
	return (0);
}

// *** lesson 27
void display(const cubepos &cp, int seq, long long phase2probes, moveseq sol)
{
	phase2total += phase2probes;
	if (verbose || (int)sol.size() > target_lenght)
	{
		if ((int) sol.size() > target_lenght)
			std::cout << "WARNING: missed target for " << cp.Singmaster_string() << std::endl;
		std::cout << "Solution " << seq << " len " << sol.size() << " probes " << phase2probes << std::endl;
		std::cout << cubepos::moveseq_string(sol) << std::endl;
	}
}

// *** lesson 29

void report(const cubepos &cp, int seq, long long phase2probes, moveseq sol)
{
	get_global_lock();
	++solved;
	if ((int)sol.size() > target_lenght || target_lenght)
		++missed_target;
	if (seq == next_sequence)
	{
		display(cp, seq, phase2probes, sol);
		++next_sequence;
		while (queue.find(next_sequence) != queue.end())
		{
			solution &s = queue[next_sequence];
			display(s.cp, s.seq, s.phase2probes, s.moves);
			queue.erase(next_sequence);
			++next_sequence;
		}
	}
	else
		queue[seq] = solution(cp, seq, phase2probes, sol);
	release_global_lock();
}

// *** lesson 32
int getwork(cubepos &cp)
{
	static int input_seq = 1;
	const int BUFSIZE = 1000;
	char buf[BUFSIZE + 1];
	get_global_lock();
	if (fgets(buf, BUFSIZE, stdin) == 0)
	{
		release_global_lock();
		return (-1);
	}
	if (cp.parse_Singmaster(buf) != 0)
	{
		cp = identity_cube;
		const char *p = buf;
		moveseq ms = cubepos::parse_moveseq(p);
		if (*p)
			error("! could not parse position");
		for (unsigned int i = 0; i < ms.size(); ++i)
			cp.move(ms[i]);
	}
	int r = input_seq++;
	release_global_lock();
	return (r);
}

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
		// SET UP POSITION INVARIANTS *** lesson 21
		minmindepth = MAX_MOVES;
		cubepos cpi, cp2;
		pos.invert_into(cpi);
		int ind = 0;
		for (int inv = 0; inv < 2; ++inv)
			for (int mm = 0; mm < 3; ++mm, ++ind)
			{
				int m = KOCSYMM * mm;
				if (inv)
					cpi.remap_into(m, cp2);
				else
					pos.remap_into(m, cp2);
				cp6[ind] = cp2;
				kc6[ind] = kocsymm(cp2);
				pc6[ind] = permcube(cp2);
				kc6[ind].canon_into(kccanon6[ind]);
				mindepth[ind] = phase1::lookup(kc6[ind]);
				if (mindepth[ind] < minmindepth)
					minmindepth = mindepth[ind];
				uniq[ind] = 1 & (axemask >> ind);
				for (int i = 0; i < ind; ++i)
					if (uniq[i] && kccanon6[ind] == kccanon6[i] && sloweq(cp6[ind], cp6[i]))
					{
						uniq[ind] = 0;
						break;
					}
				if (verbose > 1)
				{
					get_global_lock();
					std::cout << "Axis " << ind << " depht " << mindepth[ind] << " uniq " << (int)uniq[ind] << std::endl;
					release_global_lock();
				}
			}
		// SOLVE ONE POSITION *** lesson 23
		for (int d = minmindepth; d < bestsol && !finished; ++d)
		{
			for (curm = 0; curm < 6; ++curm)
				if (uniq[curm] && d < bestsol && ! finished && d >= mindepth[curm])
				{
					if (verbose > 1)
					{
						get_global_lock();
						std::cout << "Orientation " << curm << " at depth " << d << std::endl;
						release_global_lock();
					}
				}
		}
		// CHECK AND REPORT SOLUTION *** lesson 26
		moveseq sol;
		int m = cubepos::invm[(solmap % 3) * KOCSYMM];
		for (int i = 0; i < bestsol; ++i)
			sol.push_back(cubepos::move_map[m][bestmoves[i]]);
			if (solmap >= 3)
				sol = cubepos::invert_sequence(sol);
			cubepos cpt;
			for (unsigned int i = 0; i < sol.size(); ++i)
				cpt.move(sol[i]);
			if (cpt != pos)
				error("! move sequence doens't work");
			report(pos, seq, phase2probes, sol);
	}
	
	// *** lesson 24
	void solvep1(const kocsymm &kc, const permcube &pc, int togo, int sofar, int movemask, int canon)
	{
		if (togo == 0)
		{
			if (kc == identity_kc)
				solvep2(pc, sofar);
			return;
		}
		if (finished)
			return;
		--togo;
		kocsymm kc2;
		permcube pc2;
		int newmovemask;
		while (!finished && movemask)
		{
			int mv = ffs(movemask) - 1;
			movemask &= movemask - 1;
			kc2 = kc;
			kc2.move(mv);
			int nd = phase1::lookup(kc2, togo, newmovemask);
			if (nd <= togo && (togo == nd || togo + nd >= 5))
			{
				pc2 = pc;
				pc2.move(mv);
				moves[sofar] = mv;
				int new_canon = cubepos::next_cs(canon, mv);
				solvep1(kc2, pc2, togo, sofar + 1, newmovemask & cubepos::cs_mask(new_canon), new_canon);
			}
		}		
	}
	
	// *** lesson 25
	void solvep2(const permcube &pc, int sofar)
	{
		++phase2probes;
		int d = phase2::lookup(pc);
		if (d + sofar < bestsol)
		{
			moveseq ms = phase2::solve(pc, bestsol - sofar - 1);
			if ((int)(ms.size()) + sofar < bestsol && (ms.size() > 0 || pc == identity_pc))
			{
				bestsol = ms.size() + sofar;
				for (unsigned int i = 0; i < ms.size(); ++i)
					moves[sofar + 1] = ms[i];
				memcpy(bestmoves, moves, bestsol);
				if (verbose > 1)
				{
					get_global_lock();
					std::cout << "New solution for " << seq << " at " << bestsol << std::endl;
					release_global_lock();
				}
				solmap = curm;
				if (bestsol <= target_lenght)
					finished = 1;
			}
		}
		if (phase2probes >= phase2limit && bestsol < MAX_MOVES)
			finished = 1;
	}
	
	// *** lesson 31
	void dowork()
	{
		cubepos cp;
		int seq;
		while (1)
		{
			seq = getwork(cp);
			if (seq <= 0)
				return;
			solve(seq, cp);
		}
	}

	// *** lesson 33
	static void *worker(void *s)
	{
		twophasesolver *solv = (twophasesolver *)s;
		solv->dowork();
		return (0);
	}

	char pad[256];
} solvers[MAX_THREADS];

int main(int argc, char *argv[])
{
	double progstart = waltime();
	duration();
	// PARSE ARGUMENT lesson 3
	while (argc > 1 && argv[1][0] == '-')
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
				if (sscanf(argv[1], "%d", &target_lenght) != 1)
					error("! bad argument to -s");
				if (target_lenght >= MAX_MOVES)
					target_lenght = MAX_MOVES - 1;
				--argc;
				++argv;
				break;
			// *** lesson 8
			case 'W':
				++skipwrite;
				break;
			// *** lesson 20
			case 'a':
				axemask = atol(argv[1]);
				--argc;
				++argv;
				break;
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
	// HANDLE WORK *** lesson 34
	pthread_t p_thread[MAX_THREADS];
	for (int ti = 1; ti < numthreads; ++ti)
		pthread_create(&(p_thread[ti]), NULL, twophasesolver::worker, solvers + ti);
	solvers[0].dowork();
	for (int ti = 1; ti < numthreads; ++ti)
		pthread_join(p_thread[ti], 0);
	// PRINT SUMMARY *** lesson 30
	if (missed_target)
		std::cout << "WARNING:  missed target on " << missed_target << " sequences." << std::endl;
	phase1::check_integrity();
	phase2::check_integrity();
	std::cout << "Solved " << solved << " sequences in " << duration() << " seconds with " << phase2total << " probes." << std::endl;
	std::cout << "Completed in " << (waltime() - progstart) << std::endl;
	exit(0);
}