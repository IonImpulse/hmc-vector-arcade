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
    Projectile():  object2D::object2D(-260,-260,0) {
    }

    Projectile(float startx, float starty, float startSize, float angle): 
                object2D::object2D(startx,starty,startSize) {
        vel.x = 10*cos(angle); 
        vel.y = 10*sin(angle);
    }

    void drawProj() {
        object2D::drawObject();
    }
    
    
};


#endif