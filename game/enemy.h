#ifndef ENEMY
#define ENEMY 

#include <iostream>
#include <chrono>
#include <unistd.h>
#include <bitset>
#include <math.h>
#include <string.h>
#include "wrapper.cpp"
#include "basics.h"


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
        Vec2 path[4];
        Vec2 Velocity;
        float x = 0;
        float y = 256;
        const float ENEMY_SIZE = 10;

        Enemy(float startx, float starty, Vec2 inPath[4]) {
            path[4] = *inPath;
            Velocity.x = (startx-inPath[0].x)*.1;
            Velocity.y = (starty-inPath[0].y)*.1;
            x = startx;
            y = starty;
        }

    void drawEnemy() {
       
        absolute_vec(x, y, 0);
        relative_vec(ENEMY_SIZE, 0, 900);
        relative_vec(-(ENEMY_SIZE/2), ENEMY_SIZE, 1023);
        absolute_vec(x, y, 1023);   
    }
    void updateEnemy() {
        x += Velocity.x; 
        y += Velocity.y;
        
    }

};










#endif