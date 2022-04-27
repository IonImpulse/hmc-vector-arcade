#ifndef ENEMY
#define ENEMY 

#include <iostream>
#include <chrono>
#include <unistd.h>
#include <bitset>
#include <math.h>
#include <string.h>

#include "basics.h"
#include "projectile.h"
#include "rawio.h"


class Enemy : public object2D {
public: 
    Vec2 path[5];
    Vec2 Velocity;
    int x = 0;
    int y = 10;
    const int ENEMY_SIZE = 10;
    bool shooting;
    Projectile proj = Projectile();
    int life = 2;
    int lifeMax = 2;

    Enemy(float startx, float starty, float startSize, std::string inName);

    void drawObject();

    void updatePhysics();

    void shoot();

    void hitReact();
};

#endif
