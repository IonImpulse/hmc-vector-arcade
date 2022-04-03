#ifndef ENEMY
#define ENEMY 

#include <iostream>
#include <chrono>
#include <unistd.h>
#include <bitset>
#include <math.h>
#include <string.h>
#include "basics.h"

#include "rawio.h"


// typedef struct {
//     //maximum 10 point "path" (list of 10 vertices)
//     Vec2 path[4] = {point1,point2,point3,point4};
//     Vec2 Velocity;
//     float x = 0;
//     float y = 256;
//     float ENEMY_SIZE = 10;
// } Enemy;


class Enemy {
public: 
    Vec2 path[5];
    Vec2 Velocity;
    int x = 0;
    int y = 256;
    const int ENEMY_SIZE = 10;

    Enemy(float startx, float starty, Vec2 inPath[4]);

    void drawEnemy();

    void updateEnemy();

};

#endif
