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
    bool invuln = false;
    Projectile proj = Projectile();
    int life = 5;
    
    Player(float startx, float starty, float startSize, std::string inName);

    void drawObject();
    
    void shoot();

    void updatePhysics();

    void hitReact();
};



#endif
