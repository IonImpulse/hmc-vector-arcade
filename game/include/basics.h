#ifndef BASICS
#define BASICS 
#include "rawio.h"


#define SCENE_SIZE 20

#define PLAYER_SIZE 30
#define PLAYER_SPEED 15
#define PROJECTILE_SPEED 40

// vector sim -256 to 256
//cabinet is -511 to 511
#define TOP_Y 511
#define BOT_Y -511 
#define LEFT_X -511
#define RIGHT_X 511 
#define SPLIT 0

//20
#define FRAME_DELAY_MS 70
#define FRAME_DELAY (FRAME_DELAY_MS / 100.0)
#define FPS 50

typedef struct {
    float x = 0;
    float y = 0;
} Vec2;


class object2D {
    public:
        std::string name;
        Vec2 pos;
        Vec2 vel; 
        bool visibility = true;
        float ACCELERATION = 1; 
        float SIZE = 20;

    object2D();
    object2D(float startx, float starty, float size, std::string inName);
    virtual ~object2D();

    virtual void drawObject();

    virtual void updatePhysics();
    
    void handleCollision();

    virtual void hitReact();

};

#endif
