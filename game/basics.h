#ifndef BASICS
#define BASICS 
#include <string>
#include "wrapper.cpp"


#define SCENE_SIZE 20
#define FRAME_DELAY .03


typedef struct {
    float x = 0;
    float y = 0;
} Vec2;


class object2D {
    public:
        std::string name;
        Vec2 pos;
        Vec2 vel; 
        float ACCELERATION = 1; 
        float SIZE = 20;

    object2D(float startx, float starty, float size, std::string inName)  
        : SIZE{size} { 
        name = inName;
        pos.x =startx;
        pos.y = starty;
    }
    object2D() {
        Vec2 pos = {1,2};
        Vec2 vel = {0,0}; 
        float ACCELERATION = 1; 
        float SIZE = 20;
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

    void handleCollision() {
        std::cerr <<  name;
        std::cerr <<  " was hit" << std::endl;
    }

};

#endif