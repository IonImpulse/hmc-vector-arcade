#include <math.h>

#include "include/basics.h"
#include "include/projectile.h"

Projectile::Projectile():  object2D::object2D(-260,-260,3) { }

Projectile::Projectile(float startx, float starty, float startSize, float angle):
            object2D::object2D(startx,starty,startSize) {
    vel.x = 10*cos(angle); 
    vel.y = 10*sin(angle);
}

void Projectile::drawProj() {
    object2D::drawObject();
}
bool Projectile::updateProj() {
     
    pos.x += vel.x;
    pos.y += vel.y;

    // out of bounds 
    if(pos.y > ( 256 + SIZE) || pos.y < -( 256 + SIZE)) {
         
        vel.x = 0;
        vel.y = 0;
        return false;
    }  
    if(pos.x > ( 256 + SIZE) || pos.x < -( 256 + SIZE)) {
        vel.x = 0;
        vel.y = 0;
        return false;
    }  
    return true;

}
