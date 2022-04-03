#include <cmath>
#include <iostream>
#include <cstdio>
#include <climits>
#include "util.hpp"

using std::cout;

// NOTE ON CONVENTIONS
// Points are listed in a clockwise order
// The y axis faces up, while the z axis goes into the plane of the screen
// We use a left-handed coordinate system


vec4 operator*(mat4& m, vec4& vec) {
    float v[] = {vec.x, vec.y, vec.z, vec.w};

    return vec4 {
        m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2] + m[0][3]*v[3],
        m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2] + m[1][3]*v[3],
        m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2] + m[2][3]*v[3],
        m[3][0]*v[0] + m[3][1]*v[1] + m[3][2]*v[2] + m[3][3]*v[3]
    };
}



mat4 operator*(mat4 m1, mat4 m2) {
    return mat4 {
        {
            m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0] + m1[0][3]*m2[3][0],
            m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1] + m1[0][3]*m2[3][1],
            m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2] + m1[0][3]*m2[3][2],
            m1[0][0]*m2[0][3] + m1[0][1]*m2[1][3] + m1[0][2]*m2[2][3] + m1[0][3]*m2[3][3],
        }
    };
}
