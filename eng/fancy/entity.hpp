#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

#include "pga.hpp"

using namespace Terathon;

struct Model {
    const int numPoints;
    const Points3D **points;
    const int numLines;
    const int (**connections)[2];
    const float radius; 
}

struct Entity {
    const Model& model;
    Motor4D orientation;
    Point3D absPos;

    Entity(const Model& model, const Point3D& position) : 
        model{model},
        absPos{position}, 
        orientation{Motor4D(0,0,0,1,0,0,0,1)}
        {
    }
}

struct Ship {
    Point3D origin;
    Vector3D x;
    Vector3D y;
    Vector3D z;
}

#endif
