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


class Enemy : public object2D
{
    public: 
        Vec2 path[4];
        

        Enemy(float startx, float starty, float startSize): object2D::object2D(startx,starty,startSize) {
            vel.x = (startx)*.1;
            vel.y = (starty)*.1;
           
        }

    void drawEnemy() {
       
        absolute_vec(pos.x, pos.y, 0);
        relative_vec(SIZE, 0, 900);
        relative_vec(-(SIZE/2), SIZE, 1023);
        absolute_vec(pos.x, pos.y, 1023);   
    }
    void updateEnemy() {
        pos.x += vel.x; 
        pos.y += vel.y;
        
    }

};










#endif