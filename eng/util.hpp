#pragma once

#include <vector>

struct vec3 {
    float x, y, z;
};

struct mat3 {
    float elems[3][3];
};

struct vec4 {
    float x, y, z, w;
};

struct mat4 {
    float elems[4][4];
};

struct triangle {
    vec3 points[3];
};

struct game_object {
    std::vector<triangle> tris;
};
