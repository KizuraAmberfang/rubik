#include "cubepos.hpp"
#include <iostream>
#include <map>

unsigned int allpos[] = {1, 18, 243, 3240, 43239, 574908, 7618438, 100803036, 1332343288};
unsigned int c48pos[] = {1, 2, 9, 75, 934, 12077, 159131, 2101575, 27762103, 366611212};
unsigned int c96pos[] = {1, 2, 8, 48, 509, 6198, 80178, 1053077, 13890036, 183339529};

const unsigned int MAXELEMENTS = 100000;
std::map<cubepos, int> world;
std::vector<cubepos> q;

// check if the cubepos are equal, if not print differences and exit! *** lesson 77
void check(const cubepos & cp1, const cubepos & cp2, const char *msg)
{
    if (cp1 == cp2)
        return;
    for (int i = 0; i < 8; ++i)
        std::cout << " " << (int)(cp1.c[i]) << " " << (int)(cp2.c[i]) << std::endl;
    for (int i = 0; i < 12; ++i)
        std::cout << " " << (int)(cp1.e[i]) << " " << (int)(cp2.e[i]) << std::endl;
    std::cout << std::endl << msg << std::endl;
    exit(10);
}
// recursive routine *** lesson 78
void recur1(const cubepos &cp, int togo, int canonstate, std::vector<cubepos> & a)
{
    a.push_back(cp);
    if (togo--)
    {
        cubepos cp2;
        int mask = cubepos::cs_mask(canonstate);
        for (int mv = 0; mv < NMOVES; ++mv)
        {
            if ((mask >> mv) & 1)
            {
                cp2 = cp;
                cp2.move(mv);
                recur1(cp2, togo, cubepos::next_cs(canonstate, mv), a);
            }
        }
    }
}
//random sequence *** lesson 80
moveseq random_moveseq(int len)
{
    moveseq r;
    for (int i = 0; i < len; ++i)
        r.push_back(random_move());
    return (r);
}

int main(int argc, char *argv[])
{
    cubepos cp, cp2, cp3, cp4;
    // basic tests
    
    // move tests

    // inversion tests

    // multiplication tests

    // move parsing tests

    // singmaster tests

    // symmetry tests

    // breadth-first search one

    // breadth-first search two

    // breadth-first search three

    // depth-first search one
}