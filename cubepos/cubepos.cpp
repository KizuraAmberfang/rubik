#include "cubepos.hpp"
#include <iostream>
#include <math.h>
 
#define ROT2(cc, a, b) {unsigned char t = cc[a]; cc[a] = cc[b]; cc[b] = t;}
#define ROT4(cc, a, b, c, d) {unsigned char t = cc[d]; cc[d] = cc[c]; cc[c] = cc[b]; cc[b] = cc[a]; cc[a] = t;}
#define ROT22(cc, a, b, c, d) ROT2(cc,a,c) ROT2(cc,b,d)
#define EDGE4FLIP(a, b, c, d)   \
{                               \
    unsigned char t = e[d];     \
    e[d] = edge_flip(e[c]);     \
    e[c] = edge_flip(e[b]);     \
    e[b] = edge_flip(e[a]);     \
    e[a] = edge_flip(t);        \
}
#define CORNER4FLIP(a, b, cc, d)    \
{                                   \
    unsigned char t = c[d];         \
    c[d] = corner_ori_inc[c[cc]];   \
    c[cc] = corner_ori_dec[c[b]];   \
    c[b] = corner_ori_inc[c[a]];    \
    c[a] = corner_ori_dec[c[t]];    \
}
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
static char static_buf[200];

static const char *sing_solved = "UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR";
static const char *const smedges[] = {"UB", "BU", "UL", "LU", "UR", "RU", "UF", "FU", "LB", "BL", "RB", "BR", "LF", "FL", "RF", "FR", "DB", "BD", "DL", "LD", "DR", "RD", "DF", "FD"};
static const char *const smcorners[] = {"UBL", "URB", "ULF", "UFR", "DLB", "DBR",  "DFL", "DRF", "LUB", "BUR", "FUL", "RUF", "BDL", "RDB", "LDF", "FDR", "BLU", "RBU", "LFU", "FRU", "LBD", "BRD", "FLD", "RFD", "ULB", "UBR", "UFL", "URF", "DBL", "DRB", "DLF", "DFR", "LBU", "BRU", "FLU", "RFU", "BLD", "RBD", "LFD", "FRD", "BUL", "RUB", "LUF", "FUR", "LDB", "BDR", "FDL", "RDF"};

const int INVALID = 99;
static unsigned char lookup_edge_cubie[FACES * FACES];
static unsigned char lookup_corner_cubie[FACES * FACES * FACES];
static unsigned char sm_corner_order[8];
static unsigned char sm_edge_order[12];
static unsigned char sm_edge_flipped[12];

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

void cubepos::movepc(int mov)
{
    switch (mov) {
        case 0: 
            ROT4(e, 0, 2, 3, 1);
            ROT4(c, 0, 1, 3, 2);
            break;
        case 1:
            ROT22(e, 0, 2, 3, 1);
            ROT22(c, 0, 1, 3, 2);
            break;
        case 2:
            ROT4(e, 1, 3, 2, 0);
            ROT4(c, 2, 3, 1, 0);
            break;
        case 3: 
            ROT4(e, 3, 7, 11, 6);
            CORNER4FLIP(3, 7, 6, 2);
            break;
        case 4:
            ROT22(e, 3, 7, 11, 6);
            ROT22(c, 2, 3, 7, 6);
            break;
        case 5:
            ROT4(e, 6, 11, 7, 3);
            CORNER4FLIP(3, 2, 6, 7);
            break;
        case 6:
            EDGE4FLIP(2, 5, 10, 7);
            CORNER4FLIP(1, 5, 7, 3);
            break;
        case 7: 
            ROT22(e, 2, 5, 10, 7);
            ROT22(c, 3, 1, 5, 7);
            break;
        case 8: 
            EDGE4FLIP(7, 10, 5, 2);
            CORNER4FLIP(1, 3, 7, 5);
            break;
        case 9: ROT4(e, 9, 11, 10, 8);
            ROT4(c, 4, 6, 7, 5);
            break;
        case 10: 
            ROT22(e, 9, 11, 10, 8);
            ROT22(c, 4, 6, 7, 5);
            break;
        case 11: 
            ROT4(e, 8, 10, 11, 9);
            ROT4(c, 5, 7, 6, 4);
            break;
        case 12: 
            ROT4(e, 0, 4, 8, 5);
            CORNER4FLIP(0, 4, 5, 1);
            break;
        case 13: 
            ROT22(e, 0, 4, 8, 5);
            ROT22(c, 1, 0, 4, 5);
            break;
        case 14: 
            ROT4(e, 5, 8, 4, 0);
            CORNER4FLIP(0, 1, 5, 4);
            break;
        case 15: 
            EDGE4FLIP(1, 6, 9, 4);
            CORNER4FLIP(2, 6, 4, 0);
            break;
        case 16: 
            ROT22(e, 1, 6, 9, 4);
            ROT22(c, 0, 2, 6, 4);
            break;
        case 17: 
            EDGE4FLIP(4, 9, 6, 1);
            CORNER4FLIP(2, 0, 4, 6);
            break;
    }
}

void cubepos::mul(const cubepos &a, const cubepos &b, cubepos &r)
{
    for (int i = 0; i < 8; ++i)
    {
        int cc = a.c[i];
        r.c[i] = corner_ori_add(b.c[corner_perm(cc)], cc);
    }
    for (int i = 0; i < 12; ++i)
    {
        int cc = a.e[i];
        r.e[i] = edge_ori_add(b.e[edge_perm(cc)], cc);
    }
}

void cubepos::skip_whitespace(const char *&p)
{
    while (*p && *p <= ' ')
        ++p;
}

int cubepos::parse_face(const char *&p)
{
    int f = parse_face(*p);
    if (f >= 0)
        ++p;
    return (f);
}

int cubepos::parse_face(char f)
{
    switch (f) {
        case 'u':
        case 'U':
            return 0;
        case 'f':
        case 'F':
            return 1;
        case 'r':
        case 'R':
            return 2;
        case 'd':
        case 'D':
            return 3;
        case 'b':
        case 'B':
            return 4;
        case 'l':
        case 'L':
            return 5;
        default:
            return -1;
    }
}

int cubepos::parse_move(const char *&p)
{
    skip_whitespace(p);
    const char *q = p;
    int f = parse_face(q);
    if (f < 0)
        return (-1);
    int t = 0;
    switch (*q) {
        case '1':
        case '+':
            t = 0;
            break;
        case '2':
            t = 1;
            break;
        case '3':
        case '\'':
        case '-':
            t = TWIST - 1;
            break;
        default:
            return (-1);
    }
    p = q + 1;
    return ((f * TWIST) + t);
}

void cubepos::append_move(char *&p, int mv)
{
    append_face(p, mv/TWIST);
    *p++ = "123"[mv % TWIST];
    *p = 0;
}

moveseq cubepos::parse_moveseq(const char *&p)
{
    moveseq r;
    int mv;
    while((mv = parse_move(p)) >= 0)
        r.push_back(mv);
    return (r);
}

void cubepos::append_moveseq(char *&p, const moveseq &seq)
{
    *p = 0;
    for (unsigned int i = 0; i < seq.size(); ++i)
        append_move(p, seq[i]);
}

char *cubepos::moveseq_string(const moveseq &seq)
{
    if (seq.size() > 65)
        error("! can't print a move sequence that long");
    char *p = static_buf;
    append_moveseq(p, seq);
    return (static_buf);
}

static int parse_cubie(const char *&p)
{
    cubepos::skip_whitespace(p);
    int v = 1;
    int f = 0;
    while ((f = cubepos::parse_face(p)) >= 0)
    {
        v = v * 6 + f;
        if (v >= 2 * 6 * 6 * 6)
            return (-1);
    }
    return (v);
}

static int parse_edge(const char *&p)
{
    int c = parse_cubie(p);
    if (c < 6 * 6 || c >= 2 * 6 * 6)
        return (-1);
    c = lookup_edge_cubie[c - 6 * 6];
    if (c == INVALID)
        return (-1);
    return (c);
}

static int parse_corner(const char *&p)
{
    int c = parse_cubie(p);
    if (c < 6 * 6 * 6 || c >= 2 * 6 * 6 * 6)
        return (-1);
    c = lookup_corner_cubie[c - 6 * 6 * 6];
    if (c == INVALID || c >= CUBIES)
        return (-1);
    return (c);
}