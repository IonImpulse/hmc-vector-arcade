#ifndef BASICS
#define BASICS 
#include <string>
#include "rawio.h"

typedef struct {
    float x = 0;
    float y = 0;
} Vec2;

typedef struct {
    Vec2 pos;
    Vec2 vel; 
    float ACCELERATION = 1; 
    float SIZE = 20;
} body;

class object2D {
    public:
        Vec2 pos;
        Vec2 vel; 
        float ACCELERATION = 1; 
        float SIZE = 20;

    object2D(float startx, float starty, float size);

    void drawObject();

};

#endif
