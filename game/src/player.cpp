#include "../include/player.h"
#include <string>

Player::Player(float startx, float starty, float startSize, std::string inName): 
            object2D::object2D(startx,starty,startSize, inName) {
            addProjectile(&proj);
}

void Player::drawPlayer() {
    object2D::drawObject();
}

void Player::shoot() {
    proj.pos.x = pos.x + SIZE/2;
    proj.pos.y = pos.y + SIZE;
    
    proj.visibility = true;
    shooting = true;
    
    proj.vel.x = 0 + vel.x*.2;
    proj.vel.y = 10 + vel.y*.2;
}

void Player::updatePhysics() {
    // player 
    pos.x += vel.x;
    pos.y += vel.y;

    if(pos.x < LEFT_X) {
        pos.x = LEFT_X;
        vel.x = 0;
    }
    if((pos.x + SIZE) > RIGHT_X) {
        pos.x = RIGHT_X - SIZE;
        vel.x = 0;
    }
    if(pos.y < BOT_Y) {
        pos.y = -256;
        vel.y = 0;
    }
    if((pos.y + SIZE) > SPLIT) {
        pos.y = SPLIT - SIZE;
        vel.y = SPLIT;
    }

    vel.x *= .9;
    vel.y *= .9;

    /// Projectile 
    shooting = proj.updateProj();

    }
