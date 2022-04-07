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
    Projectile();
    Projectile(float startx, float starty, float startSize, float angle);

    void drawProj();

    bool updateProj();
};


#endif
