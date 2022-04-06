#ifndef BASICS
#define BASICS 
#include <string>
#include "rawio.h"


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

<<<<<<< HEAD
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
=======
    object2D(float startx, float starty, float size);
>>>>>>> 59545079acc6b73c55c3559f0277c06a6838e1d0

    void drawObject();

    void handleCollision() {
        std::cerr <<  name;
        std::cerr <<  " was hit" << std::endl;
    }

};

#endif
