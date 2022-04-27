#include "../include/player.h"
#include "../include/scene.h"
#include <string>
#include <iostream>
#include <math.h>

Player::Player(float startx, float starty, float startSize, std::string inName): 
            object2D::object2D(startx,starty,startSize, inName) {
            
}

void Player::drawObject() {
    
    if (invuln){
        draw_absolute_vector(pos.x, pos.y, 0);
        draw_absolute_vector(pos.x + SIZE, pos.y, 500);
        draw_absolute_vector(pos.x + SIZE, pos.y + SIZE, 500);
        draw_absolute_vector(pos.x, pos.y + SIZE, 500);
        draw_absolute_vector(pos.x, pos.y, 500);
    } else {
        object2D::drawObject();
    }

}

void Player::hitReact() {
    invuln = true;
    invTrigger = getFrame() + 10;
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


    // Player random walk since input doesnt work 
    if ((getFrame() % 20 )== 0) {
        float angle = (rand() % (2*314))*.01;
        std::cerr << angle;
        vel.x += 10*cos(angle);
        vel.y += 10*sin(angle);
    }
    //
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
    shooting = proj.visibility;

    //invulnerability
    if (invTrigger <= getFrame()){
        invuln = false; 
    }


    }
