#ifndef PROJECTILE
#define PROJECTILE
#include "basics.h"
#include <string>

class Projectile : public object2D
{
    public:
        //int team = 1;
    // maybe fix angle stuff 
    // angle in radians
    Projectile():  object2D::object2D(-260,-260,3, "p") {
    }

    Projectile(float startx, float starty, float startSize, float angle): 
                object2D::object2D(startx,starty,startSize, "p") {
        vel.x = 10*cos(angle); 
        vel.y = 10*sin(angle);
    }

    void drawProj() {
        object2D::drawObject();
    }

    bool updateProj() {
         
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
    
    
};


#endif