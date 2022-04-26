#include "../include/enemy.h"
#include "../include/basics.h"
#include "../include/scene.h"

Enemy::Enemy(float startx, float starty, float startSize, std::string inName)
    : object2D::object2D(startx,starty,startSize, inName)
{
    vel.x = (startx)*.2;
    vel.y = (starty)*.2;
    

}

void Enemy::drawObject() {
    draw_absolute_vector(pos.x, pos.y+ SIZE, 0);
    draw_relative_vector(SIZE/2, -SIZE, 1023);
    draw_relative_vector(SIZE/2, SIZE, 1023);
    draw_absolute_vector(pos.x + SIZE, pos.y+ SIZE, 1023);   

    
}


void Enemy::updatePhysics() {


    pos.x += vel.x; 
    pos.y += vel.y;
 
    
    if(pos.x < LEFT_X) {
        pos.x = LEFT_X;
        vel.x = -vel.x;
    }
    if((pos.x + SIZE) > RIGHT_X) {
        pos.x = RIGHT_X - SIZE;
        vel.x = -vel.x;
    }
    if((pos.y + SIZE) > TOP_Y) {

        pos.y = TOP_Y - SIZE;
        vel.y = -vel.y;
    }
    if((pos.y) < SPLIT) {
        pos.y = SPLIT ;
        vel.y = -vel.y;
    }

    if(!shooting && pos.y > SPLIT + 100) {
        shoot();
    }
    
    shooting = proj.visibility;
}
 void Enemy::shoot() {
    proj.pos.x = pos.x + SIZE/2;
    proj.pos.y = pos.y - proj.SIZE;
    proj.vel.x = 0+.5*vel.x; 
    proj.vel.y = -10+.5*vel.y; 
    proj.visibility = true;
    shooting = true;
}
