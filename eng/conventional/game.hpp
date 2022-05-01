#ifndef GAME_HPP
#define GAME_HPP

#include "util.hpp"

class Model {
    public:
        const int num_points;
        const vec3* points;
        const int num_connections;
        const int* connections; // Please do not mess up the format. This will pretty easily create UB if you do
};

class Entity {
    public:
        const Model& model;
        mat4 orientation;
        vec3 position;
};

#endif
