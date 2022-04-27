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

    int life = 5;
    bool invuln = false;
    unsigned long long int invTrigger = 0;
    
    Player(float startx, float starty, float startSize, std::string inName);

    void drawObject();
    
    void shoot();

    void updatePhysics();

    void hitReact();
};



#endif
