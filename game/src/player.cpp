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

    shooting = true;
    proj.vel.x = 0 + vel.x*.2;
    proj.vel.y = 10 + vel.y*.2;
}

void Player::updatePhysics() {
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
    shooting = proj.updateProj();

    }
