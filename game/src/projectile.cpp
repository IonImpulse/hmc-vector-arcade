#include <math.h>

#include "../include/basics.h"
#include "../include/projectile.h"

Projectile::Projectile():  object2D::object2D(-260,-260,PROJECTILE_SIZE, "proj") {visibility = false;}

Projectile::Projectile(float startx, float starty, float startSize, float angle):
            object2D::object2D(startx,starty,startSize, "proj") {
    vel.x = 10*cos(angle); 
    vel.y = 10*sin(angle);
    visibility = false;
}

void Projectile::drawProj() {
    visibility = true;



    if (visibility) {
        object2D::drawObject();
    }  
}

void Projectile::hitReact() {

}

void Projectile::updatePhysics() {
     
    pos.x += vel.x;
    pos.y += vel.y;

    // out of bounds 
    if(pos.y > ( TOP_Y - SIZE) || pos.y < BOT_Y) {
        vel.x = 0;
        vel.y = 0;
        visibility = false;
        
    }  
    if(pos.x > ( RIGHT_X - SIZE) || pos.x < LEFT_X ) {
        vel.x = 0;
        vel.y = 0;
        visibility = false;
        
    }  
  

}
