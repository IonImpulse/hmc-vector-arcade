#ifndef PLAYER
#define PLAYER
#include "basics.h"
#include <string>
#include "projectile.h"

class Player : public object2D
{
    public:
        //int team = 1;
        bool shooting = false;
        Projectile proj = Projectile();
    
    Player(float startx, float starty, float startSize): 
                object2D::object2D(startx,starty,startSize) {
    }

    void drawPlayer() {
        object2D::drawObject();
    }
    
    void shoot() {
        proj = Projectile(pos.x + SIZE/2, pos.y + SIZE, 3, 1.5707);
        shooting = true;
        proj.vel.x += vel.x*.1;
        proj.vel.y += vel.y*.1;
    }

    void updatePhysics() {
    // player 
    pos.x += vel.x;
    pos.y += vel.y;

    if(pos.x < -256) {
        pos.x = -256;
        vel.x = 0;
    }
    if((pos.x + SIZE) > 256) {
        pos.x = 256 - SIZE;
        vel.x = 0;
    }
    if(pos.y < -256) {
        pos.y = -256;
        vel.y = 0;
    }
    if((pos.y + SIZE) > 0) {
        pos.y = 0 - SIZE;
        vel.y = 0;
    }

    vel.x *= .9;
    vel.y *= .9;

    /// Projectile 
    if (shooting) {
        proj.pos.x += proj.vel.x;
        proj.pos.y += proj.vel.y;

        // out of bounds 
        if(proj.pos.y > ( 256 + proj.SIZE) || proj.pos.y < -( 256 + proj.SIZE)) {
            shooting = false; 
            proj.vel.x = 0;
            proj.vel.y = 0;
        }  
        if(proj.pos.x > ( 256 + proj.SIZE) || proj.pos.x < -( 256 + proj.SIZE)) {
            shooting = false; 
            proj.vel.x = 0;
            proj.vel.y = 0;
        }  

    }
    }




};



#endif