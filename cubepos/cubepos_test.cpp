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
    if (sizeof(int) != 4)
        error("! this code assumes a 4-byte int throughout");
    if (sizeof(short) != 2)
        error("! this code assumes a 2-byte short");
    if (sizeof(cubepos) != 20)
        error("! size of cubepos is not 20");
    if (lrand48() == 0)
        srand48(getpid() + time(0));
    for (int i = 0; i < 8; ++i)
        if (cp.c[i] != identity_cube.c[i])
            error("! bad initial cp");
    for (int i = 0; i < 12; ++i)
        if (cp.e[i] != identity_cube.e[i])
            error("! bad initial cp");
    for (int i = 0; i < 16; ++i)
        if ((cubepos::face_map[i][0] % 3) != 0)
            error("! up down not preserved in first 16");
    // move tests
    std::cout << "Verifying f/b moves." << std::endl;
    for (int i = 0; i < NMOVES; ++i)
    {
        cp.move(i);
        cp.movepc(i);
        check(cp, identity_cube, "problem veryfing fb of moves");
    }
    std::cout << "Verifying forward move." << std::endl;
    for (int i = 0; i < FACES; ++i)
    {
        for (int j = 0; j < 4; ++j)
            cp.move(i * TWIST);
        check(cp, identity_cube, "problem verifying order of basic generators");
    }
    std::cout << "Verifying bw moves." << std::endl;
    for (int i = 0; i < FACES; ++i)
    {
        for (int j = 0; j < 4; ++j)
            cp.movepc(i * TWIST);
        check(cp, identity_cube, "problem verifying order of basic generators 2");
    }
    // inversion tests
    std::cout << "Random cube inversion" << std::endl;
    for (int i = 0; i < 100; ++i)
    {
        cp.randomize();
        cp.invert_into(cp2);
        cp2.invert_into(cp3);
        check(cp, cp3, "Inversion failed");
    }
    std::cout << "Move inversion" << std::endl;
    for (int i = 0; i < 100; ++i)
    {
        moveseq ms = random_moveseq(10);
        moveseq msi = cubepos::invert_sequence(ms);
        cp = identity_cube;
        cp2 = identity_cube;
        for (unsigned int k = 0; k < ms.size(); ++k)
        {
            cp.move(ms[k]);
            cp2.move(msi[k]);
        }
        cp.invert_into(cp3);
        check(cp2, cp3, "Invert move sequence failed");
    }
    // multiplication tests
    std::cout << "Multiplication" << std::endl;
    for (int i = 0; i < 100; ++i)
    {
        moveseq ms = random_moveseq(10), ms2 = random_moveseq(10);
        cp = identity_cube;
        cp2 = identity_cube;
        cp3 = identity_cube;
        for (unsigned int k = 0; k < ms.size(); ++k)
        {
            cp.move(ms[k]);
            cp3.move(ms[k]);
        }
        for (unsigned int k = 0; k < ms2.size(); ++k)
        {
            cp2.move(ms2[k]);
            cp3.move(ms2[k]);
        }
        cubepos::mul(cp, cp2, cp4);
        check(cp4, cp3, "Bad product");
    }
    // move parsing tests

    // singmaster tests

    // symmetry tests

    // breadth-first search one

    // breadth-first search two

    // breadth-first search three

    // depth-first search one
}