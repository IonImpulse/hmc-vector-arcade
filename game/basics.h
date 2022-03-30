#ifndef BASICS
#define BASICS 
#include <string>
#include "wrapper.cpp"

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
        absolute_vec(pos.x, pos.y, 0);
        absolute_vec(pos.x + SIZE, pos.y, 1023);
        absolute_vec(pos.x + SIZE, pos.y + SIZE, 1023);
        absolute_vec(pos.x, pos.y + SIZE, 1023);
        absolute_vec(pos.x, pos.y, 1023);
        absolute_vec(-250,0,0);
        absolute_vec(250,0,255);
    }

};

#endif