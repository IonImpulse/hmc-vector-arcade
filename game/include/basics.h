#ifndef BASICS
#define BASICS 
#include "rawio.h"


#define SCENE_SIZE 20

#define FRAME_DELAY_MS 30
#define FRAME_DELAY (FRAME_DELAY_MS / 100.0)

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

    object2D();
    object2D(float startx, float starty, float size);

    void drawObject();

    void handleCollision();

};

#endif
