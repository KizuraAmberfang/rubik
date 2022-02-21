#include "cubepos.hpp"
#include <iostream>
#include <math.h>
 
 // ***  STATIC DATA INSTANTIATION  *** lesson 13
const cubepos identity_cube(0,0,0);
unsigned char cubepos::corner_ori_inc[CUBIES], cubepos::corner_ori_dec[CUBIES], cubepos::corner_ori_neg_strip[CUBIES], cubepos::mod24[CUBIES * 2];
char cubepos::faces[FACES] = {'U', 'F', 'R', 'D', 'B', 'L'}; 
unsigned char cubepos::edge_trans[NMOVES][CUBIES], cubepos::corner_trans[NMOVES][CUBIES];
// clockwise rotation of the cubes in each face
static const unsigned char edge_twist_perm[FACES][4] = {{0,2,3,1},
                                                        {3,7,11,6},
                                                        {2,5,10,7},
                                                        {9,11,10,8},
                                                        {0,4,8,5},
                                                        {1,6,9,4}};
 
static const unsigned char corner_twist_perm[FACES][4] = {  {0,1,3,2},
                                                            {2,3,7,6},
                                                            {3,1,5,7},
                                                            {4,6,7,5},
                                                            {1,0,4,5},
                                                            {0,2,6,4}};
// edge and corner orientation convention
static const unsigned char edge_change[FACES] = {0,0,1,0,0,1}; 
static const unsigned char corner_change[FACES][4] = {  {0,0,0,0},
                                                        {1,2,1,2},
                                                        {1,2,1,2},
                                                        {0,0,0,0},
                                                        {1,2,1,2},
                                                        {1,2,1,2}};

unsigned char cubepos::inv_move[NMOVES];
// ***  LOCAL ROUTINE  *** lesson 36

void cubepos::invert_into(cubepos & dst) const
{
    for (int i = 0; i < 8; ++i)
    {
        int cval = c[i];
        dst.c[corner_perm(cval)] = corner_ori_sub(i, cval);
    }
    for (int i = 0; i < 12; ++i)
    {
        int cval = e[i];
        dst.e[edge_perm(cval)] = edge_val(i, edge_ori(cval));
    }
}

 void cubepos::init()
 {
     static int initialized = 0;
     if (initialized)
        return;
    initialized = 1;
    // we initialize the cubepos *** lesson 14
    for (int i = 0; i < CUBIES; ++i)
    {
        int perm = corner_perm(i);
        int ori = corner_ori(i);
        corner_ori_inc[i] = corner_val(perm, (ori + 1) % 3);
        corner_ori_dec[i] = corner_val(perm, (ori + 2) % 3);
        corner_ori_neg_strip[i] = corner_val(0, (3 - ori) % 3);
        mod24[i] = i;
        mod24[i + CUBIES] = i;
    }
    for (int m = 0; m < NMOVES; ++m)
        for (int c = 0; c < CUBIES; ++c)
        {
            edge_trans[m][c] = c;
            corner_trans[m][c] = c;
        }
    for (int f = 0; f < FACES; ++f)
        for (int t = 0; t < 3; t++)
        {
            int m = f * TWIST + t;
            int isquarter = (t == 0 || t == 2);
            int perminc = t + 1;
            if (m < 0)
                continue;
            for (int i = 0; i < 4; ++i)
            {
                int ii = (i + perminc) % 4;
                for (int o = 0; o < 2; ++o)
                {
                    int oo = o;
                    if (isquarter)
                        oo = oo ^ edge_change[f];
                    edge_trans[m][edge_val(edge_twist_perm[f][i], o)] = edge_val(edge_twist_perm[f][ii], oo);
                }
                for (int o = 0; o < 3; ++o)
                {
                    int oo = o;
                    if (isquarter)
                        oo = (corner_change[f][i] + oo) % 3;
                    corner_trans[m][corner_val(corner_twist_perm[f][i], o)] = corner_val(corner_twist_perm[f][ii], oo);
                }
            }
        }
    for (int i = 0; i < NMOVES; ++i)
        inv_move[i] = TWIST * (i / TWIST) + (NMOVES - i - 1) % TWIST;
 }

 cubepos::cubepos(int, int, int)
 {
     for (int i = 0; i < 8; ++i)
        c[i] = corner_val(i, 0);
    for (int i = 0; i < 12; ++i)
        e[i] = edge_val(i, 0);
    init();
 }

 void cubepos::move(int mov)
 {
    const unsigned char *p = corner_trans[mov];
    c[0] = p[c[0]];
    c[1] = p[c[1]];
    c[2] = p[c[2]];
    c[3] = p[c[3]];
    c[4] = p[c[4]];
    c[5] = p[c[5]];
    c[6] = p[c[6]];
    c[7] = p[c[7]];
    p = edge_trans[mov];
    e[0] = p[e[0]];
    e[1] = p[e[1]];
    e[2] = p[e[2]];
    e[3] = p[e[3]];
    e[4] = p[e[4]];
    e[5] = p[e[5]];
    e[6] = p[e[6]];
    e[7] = p[e[7]];
    e[8] = p[e[8]];
    e[9] = p[e[9]];
    e[10] = p[e[10]];
    e[11] = p[e[11]];
 }

moveseq cubepos::invert_sequence(const moveseq &seq)
{
    unsigned int len = seq.size();
    moveseq r(len);
    for (unsigned int i = 0; i < len; ++i)
        r[len - i - 1] = invert_move(seq[i]);
    return (r);
}