#ifndef PLAYER
#define PLAYER
#include "basics.h"
#include <string>
#include "projectile.h"
#include "scene.h"

class Player : public object2D
{
public:
    //int team = 1;
    bool shooting = false;
    Projectile proj = Projectile();
    
    Player(float startx, float starty, float startSize, std::string inName);

    void drawPlayer();
    
    void shoot();

    void updatePhysics();
};



#endif
