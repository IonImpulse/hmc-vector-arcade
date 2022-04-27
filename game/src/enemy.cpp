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

    // damage 
    int brightnessRatio = 1023 - ((life/lifeMax)*1023) ;
    draw_absolute_vector(pos.x, pos.y+ SIZE, 0);
    draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, brightnessRatio);
    draw_relative_vector(-SIZE/4,+SIZE/4, brightnessRatio);
     draw_absolute_vector(pos.x + SIZE, pos.y+ SIZE, brightnessRatio);


    
}

void Enemy::hitReact() { 
    life -= 1;
    if (life == 0){ 
        life = lifeMax ; 
        visibility = false;

        // explosion 
        // draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, 0);
        // draw_relative_vector(+SIZE*2,+SIZE*2, 1024);
        //  draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, 0);
        // draw_relative_vector(-SIZE*2,-SIZE*2, 1024);
        //  draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, 0);
        // draw_relative_vector(-SIZE*2,+SIZE*2, 1024);
        //  draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, 0);
        // draw_relative_vector(+SIZE*2,-SIZE*2, 1024);
        //  draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, 0);
        // draw_relative_vector(-SIZE*4,0, 1024);
        //  draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, 0);
        // draw_relative_vector(+SIZE*4,0, 1024);
        //  draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, 0);
        // draw_relative_vector(0,+SIZE*4, 1024);
        //  draw_absolute_vector(pos.x+SIZE/2, pos.y+ SIZE/2, 0);
        // draw_relative_vector(0,-SIZE*4, 1024);


    }
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
