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
        Projectile proj = Projectile();
        bool shooting = false;      
        
        Enemy(float startx, float starty, float startSize, std::string inName): 
        object2D::object2D(startx,starty,startSize, inName) {
            vel.x = (startx)*.1;
            vel.y = (starty)*.1;
            addProjectile(&proj);

        }

    void drawEnemy() {
       
        absolute_vec(pos.x, pos.y+SIZE, 0);
        relative_vec(SIZE, 0, 900);
        relative_vec(-(SIZE/2), -SIZE, 1023);
        absolute_vec(pos.x, pos.y+SIZE, 1023);   
    }

    void updateEnemy() {
        pos.x += vel.x; 
        pos.y += vel.y;
        

        
        if(pos.x < -256) {
            pos.x = -256;
            vel.x = -vel.x;
        }
        if((pos.x + SIZE) > 256) {
            pos.x = 256 - SIZE;
            vel.x = -vel.x;
        }
        if((pos.y + SIZE) > 256) {
            pos.y = 256-SIZE;
            vel.y = -vel.y;
        }
        if((pos.y) < 0) {
            pos.y = 0 ;
            vel.y = -vel.y;
        }

        if(!shooting && pos.y > 100) {
            shoot();
        }

        shooting = proj.updateProj();




    }

     void shoot() {
        proj.pos.x = pos.x + SIZE/2;
        proj.pos.y = pos.y;
        proj.vel.x = 0+.5*vel.x; 
        proj.vel.y = -10+.5*vel.y; 
        
        shooting = true;
    }

};










#endif