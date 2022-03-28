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

    object2D(float startx, float starty, float size)  
        : SIZE{size} { 
            pos.x =startx;
        pos.y = starty;
    }

    void drawObject() {
        draw_absolute_vector(pos.x, pos.y, 0);
        draw_absolute_vector(pos.x + SIZE, pos.y, 1023);
        draw_absolute_vector(pos.x + SIZE, pos.y + SIZE, 1023);
        draw_absolute_vector(pos.x, pos.y + SIZE, 1023);
        draw_absolute_vector(pos.x, pos.y, 1023);
        draw_absolute_vector(-250,0,0);
        draw_absolute_vector(250,0,255);
    }

};

#endif
