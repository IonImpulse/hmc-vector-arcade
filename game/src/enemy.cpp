#include "include/enemy.h"
#include "include/basics.h"
#include "include/scene.h"

Enemy::Enemy(float startx, float starty, float startSize, std::string inName)
    : object2D::object2D(startx,starty,startSize)
{
    vel.x = (startx)*.1;
    vel.y = (starty)*.1;
    addProjectile(&proj);

}

void Enemy::drawEnemy() {
    draw_absolute_vector(x, y, 0);
    draw_relative_vector(ENEMY_SIZE, 0, 900);
    draw_relative_vector(-(ENEMY_SIZE/2), ENEMY_SIZE, 1023);
    draw_absolute_vector(x, y, 1023);   
}
void Enemy::updateEnemy() {
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
 void Enemy::shoot() {
    proj.pos.x = pos.x + SIZE/2;
    proj.pos.y = pos.y;
    proj.vel.x = 0+.5*vel.x; 
    proj.vel.y = -10+.5*vel.y; 
    
    shooting = true;
}
