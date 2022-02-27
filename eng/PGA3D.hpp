#pragma once

#include <cmath>
#include <cstdio>
#include <array>

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


PGA3D operator ~ (const PGA3D &a);
PGA3D operator ! (const PGA3D &a);
PGA3D operator * (const PGA3D &a, const PGA3D &b);
PGA3D operator ^ (const PGA3D &a, const PGA3D &b);
PGA3D operator & (const PGA3D &a, const PGA3D &b);
PGA3D operator | (const PGA3D &a, const PGA3D &b);
PGA3D operator + (const PGA3D &a, const PGA3D &b);
PGA3D operator - (const PGA3D &a, const PGA3D &b);
PGA3D operator * (const float& a, const PGA3D &b);
PGA3D operator * (const PGA3D &a ,const float &b);
PGA3D operator + (const float &a, const PGA3D &b);
PGA3D operator + (const PGA3D &a, const float& b);
PGA3D operator - (const float &a, const PGA3D &b);
PGA3D operator - (const PGA3D &a, const float &b);

PGA3D rotor(float angle, PGA3D line);
PGA3D translator(float dist, PGA3D line);
PGA3D plane(float a, float b , float c, float d);
