#ifndef CUBEPOS_HPP
#define CUBEPOS_HPP
#include <cstring>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>

const int NMOVES = 18; //F F2 F' etc
const int TWIST = 3; // number of twist a corner can have
const int FACES = 6; // number of faces of the cube
const int M = 48; // number of automorphism induced by rotation and reflection
const int CUBIES = 24; // number of combination for corner and edges

extern const class cubepos identity_cube; // identity of the group

// *** GLOBAL UTILITY DECLARATION *** lesson 30
typedef std::vector<int> moveseq;
// constant to move mask *** lesson 64
const int ALLMOVEMASK = (1 << NMOVES) - 1;
const int ALLMOVEMASK_EXT = (1 << NMOVES) - 1;
// constant to be efficient int the move sequence we explore
const int CANONSEQSTATES = FACES + 1;
const int CANONSEQSTART = 0;
//generic utility routine *** lesson 75
void error (const char *s);
inline double myrand()
{
    return drand48();
}
inline int random_move()
{
    return (int)(NMOVES * myrand());
}
double waltime();
double duration();

class cubepos 
{
    public: 
    // constructor

    inline cubepos(const cubepos &cp = identity_cube)
    {
        *this = cp;
    }

    cubepos(int, int, int);

    // function for comparing two cube - lesson 9
    inline bool operator< (const cubepos &cp) const
    {
        return (memcmp(this, &cp, sizeof(cp)) < 0);
    }

    inline bool operator== (const cubepos &cp) const
    {
        return (memcmp(this, &cp, sizeof(cp)) == 0);
    }

    inline bool operator!= (const cubepos &cp) const
    {
        return (memcmp(this, &cp, sizeof(cp)) != 0);
    }

    // function to find the permutation and orientation of cubies
    static inline int edge_perm(int cubieval)
    {
        // permutation of the edge: bitshifting of 1
        // example: [xxx 1001 1] -> [xxxx 1001] -> 9, this is the slot
        return (cubieval >> 1);
    }

    static inline int edge_ori(int cubieval)
    {
        // orientation of the edge: and with 1 [0000000 1]
        // example: [xxx 1001 1] & [0000000 1] = [0000000 1] -> is flipped
        return (cubieval & 1);
    }

    static inline int corner_perm(int cubieval)
    {
        // permutation of the corner: and with 7 [00000 111]
        // example: [xxx 10 010] & [00000 111] = [00000 010] -> slot 2
        return (cubieval & 7);
    }

    static inline int corner_ori(int cubieval)
    {
        // orientation of the corner: 3 bitshifting
        // example: [xxx 10 010] >> 3 = [000000 10] -> 2, counterclowise twist
        return (cubieval >> 3);
    }

    static inline int edge_flip(int cubieval)
    {
        // change flip orientation of the edge with xor operation
        // example: [xxx 1001 1] xor [0000000 1] = [xxx 1001 0]
        return (cubieval ^ 1);
    }

    static inline int edge_val(int perm, int ori)
    {
        // build the edge from his slot (perm) and orientation
        // example: slot 3 and ori 1 -> 3 * 2 + 1 = 7 -> [000 0011 1]
        return (perm * 2 + ori);
    }

    static inline int corner_val(int perm, int ori)
    {
        // build the corner from his slot (perm) and orientation
        // example: slot 5 and ori 2 -> 2 * 8 + 5 = 21 -> [000 10 101]
        return (ori * 8 + perm);
    }
  
    static inline int edge_ori_add(int cv1, int cv2)
    {
        // give the orientation of cubieval (cv2) to cv1
        // example: [000 0100 1] cv1 [000 0011 0] cv2
        //          edge_ori(cv2) = 0
        //          [000 0100 1] ^ [000 0000 0] = [000 0100 1]
        return (cv1 ^ edge_ori(cv2));
    }

    static inline int corner_ori_add(int cv1, int cv2)
    {
        // give the orientation of cv2 to cv1
        // example: [000 01 011] cv1 [000 10 101] cv2
        //          [000 10 011] & [000 11 000] = [000 10 000]
        //          [000 01 011] + [000 10 000] = [000 11 011] = 1 + 2 + 8 + 16 = 26 -> 3
        return (mod24[cv1 + (cv2 & 24)]);
    }

    static inline int corner_ori_sub(int cv1, int cv2)
    {
        return (cv1 + corner_ori_neg_strip[cv2]);
    }

    static void init();

    void move(int mov);

    static int invert_move(int mv)
    {
        return inv_move[mv];
    }

    static moveseq invert_sequence(const moveseq &seq);
    
    void invert_into(cubepos & dst) const;

    void movepc(int mov);

    // right multiplication
    static void mul(const cubepos &a, const cubepos &b, cubepos &r);

    // left multiplication
    inline static void mulpc(const cubepos &a, const cubepos &b, cubepos &r)
    {
        mul(b, a, r);
    }

    static void skip_whitespace(const char *&p);
    static int parse_face(const char *&p);
    static int parse_face(char f);

    static void append_face(char *&p, int f)
    {
        *p++ = faces[f];
    }

    static int parse_move(const char *&p);
    static void append_move(char *&p, int mv);
    static moveseq parse_moveseq(const char *&p);
    static void append_moveseq(char *&p, const moveseq &seq);
    static char *moveseq_string(const moveseq &seq);
    
    const char *parse_Singmaster(const char *p);
    char *Singmaster_string() const;

    // declaration of function to remap a position according with mpm' *** lesson 63
    void remap_into(int m, cubepos & dst) const;
    void canon_into48(cubepos & dst) const;
    void canon_into48_aux(cubepos & dst) const;
    void canon_into96(cubepos & dst) const;

    // we need a random cubie *** lesson 67

    void randomize();

    // utility routine *** lesson 74

    static inline int next_cs(int cs, int mv)
    {
        return canon_seq[cs][mv];
    }

    static inline int cs_mask(int cs)
    {
        return canon_seq_mask[cs];
    }
    
    static inline int cs_mask_ext(int cs)
    {
        return canon_seq_mask_ext[cs];
    }

    // ***  STATIC DATA DECLARATION  ** lesson 12

    // this array serve the purpose of allowing changes of orientation without performi division or modulo operation.
    static unsigned char corner_ori_inc[CUBIES], corner_ori_dec[CUBIES], corner_ori_neg_strip[CUBIES], mod24[2 * CUBIES];

    // the faces! we use U F R D B L rapresentation here
    static char faces[FACES];

    // how a move effect the cubies
    static unsigned char edge_trans[NMOVES][CUBIES], corner_trans[NMOVES][CUBIES];
    
    static unsigned char inv_move[NMOVES];
	// lesson 55
    static unsigned char face_map[M][FACES], move_map[M][NMOVES];
    static unsigned char invm[M], mm[M][M];
    static unsigned char rot_edge[M][CUBIES], rot_corner[M][CUBIES];

    // array to give next state and bit mask ** lesson 71
    static unsigned char canon_seq[CANONSEQSTATES][NMOVES];
    static int canon_seq_mask[CANONSEQSTATES];
    static int canon_seq_mask_ext[CANONSEQSTATES];

    // ***  DATA RAPRESENTATION *** lesson 7
    // the 8 corner -> the 3 low bit is the slot
    
    // top layer     bot layer
    
    //   0 - 1         4 - 5
    //   |   |         |   |
    //   2 - 3         6 - 7

    // the 4th and 5th bit is the orientation: 0 no twist, 1 clockwise twist, 2 counterclockwise twist
    // example: [xxx 00 000] -> slot 0, no twist
    //          [xxx 10 010] -> slot 2, counterclockwise twist

    unsigned char c[8]; // this rapresent the 8 corner

    // the 12 edges -> the low bit indicates if the edge is flipped: 0 is not flipped, 1 is flipped
    // 2nd, 3rd, 4th, 5th bit is the slot of the edge

    // top layer     mid layer     top layer

    //  / 0 \          4 - 5         /  8  \
    //  1   2          |   |         9     10
    //  \ 3 /          6 - 7         \  11 /   

    // example: [xxx 00 000] -> slot 0, no flip
    //          [xxx 1001 1] -> slot 9, flip

    unsigned char e[12]; // the 12 edges
};

static cubepos cubepos_initialization_hack(1,2,3);

#endif