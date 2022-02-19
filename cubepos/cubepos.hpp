#ifndef CUBEPOS_HPP
#define CUBEPOS_HPP
const int NMOVES = 18 //F F2 F' etc
const int TWIST = 3 // number of twist a corner can have
const int FACES = 6 // number of faces of the cube
const int M = 48 // number of automorphism induced by rotation and reflection
const int CUBIES = 24 // number of combination for corner and edges

extern const class cubepos identity_cube; // identity of the group

class cubepos 
{
    public: // in lesson 9
    // static data in lesson 12

    // ***  DATA RAPRESENTATION *** lesson 7
    // the 8 corner -> the 3 low bit is the slot
    
    // top layer     bot layer
    
    //   0 - 1         4 - 5
    //   |   |         |   |
    //   2 - 3         6 - 7

    // the 4th and 5th bit is the orientation: 0 no twist, 1 clockwise twist, 2 counterclockwise twist

    unsigned char c[8] // this rapresent the 8 corner

    // the 12 edges -> the low bit indicates if the edge is flipped: 0 is not flipped, 1 is flipped
    // 2nd, 3rd, 4th, 5th bit is the slot of the edge

    // top layer     mid layer     top layer

    //  / 0 \          4 - 5         /  8  \
    //  1   2          |   |         9     10
    //  \ 3 /          6 - 7         \  11 /   

    unsigned char e[12] // the 12 edges
};

// static inizialization hack lesson 16


#endif