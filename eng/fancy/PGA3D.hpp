#pragma once

#include <cmath>
#include <cstdio>
#include <array>

// Multivector class that allows us to represent points, lines, etc.
class PGA3D {
  public:
    PGA3D ()  { std::fill( mvec, mvec + sizeof( mvec )/4, 0.0f ); }
    PGA3D (float f, int idx=0) { std::fill( mvec, mvec + sizeof( mvec )/4, 0.0f ); mvec[idx] = f; }

    float& operator [] (size_t idx) { return mvec[idx]; }

    const float& operator [] (size_t idx) const { return mvec[idx]; }
    PGA3D Conjugate(); 
    PGA3D Involute();
    PGA3D log();
    float norm();
    float inorm();
    PGA3D normalized();
  private:  
    float mvec[16];
};


PGA3D operator ~ (const PGA3D &a);                     // Reverses the order of blades
PGA3D operator ! (const PGA3D &a);                     // Returns the dual of the vector
PGA3D operator * (const PGA3D &a, const PGA3D &b);     // Geometric Product
PGA3D operator ^ (const PGA3D &a, const PGA3D &b);     // Outer Product
PGA3D operator & (const PGA3D &a, const PGA3D &b);     // Regressive Product
PGA3D operator | (const PGA3D &a, const PGA3D &b);     // Inner Product
PGA3D operator + (const PGA3D &a, const PGA3D &b);     // Addition
PGA3D operator - (const PGA3D &a, const PGA3D &b);     // Subtraction
PGA3D operator * (const float& a, const PGA3D &b);     // Scalar Multiplication
PGA3D operator * (const PGA3D &a ,const float &b);     // Scalar Multiplication
PGA3D operator + (const float &a, const PGA3D &b);     // Scalar Addition
PGA3D operator + (const PGA3D &a, const float& b);     // Scalar Addition
PGA3D operator - (const float &a, const PGA3D &b);     // Scalar Subtraction
PGA3D operator - (const PGA3D &a, const float &b);     // Scalar Subtraction

PGA3D rotor(float angle, PGA3D line);                  // Returns a rotor given a line and an angle 
PGA3D translator(float dist, PGA3D line);              // Returns a translation rotor given a liene and a distance
PGA3D plane(float a, float b , float c, float d);      // Defines a plane from homogenous coordinates
