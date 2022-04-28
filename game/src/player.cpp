#include "../include/player.h"
#include "../include/scene.h"
#include <string>
#include <iostream>
#include <math.h>

#define INVTIME 1

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
    invTrigger = getFrame() + FPS*INVTIME;
    life -= 1;
    if (life == 0) { 
        gameOver();
    }
}

void Player::shoot() {
    if (visibility && !invuln) {
        proj.pos.x = pos.x + SIZE/2;
        proj.pos.y = pos.y + SIZE;
    
        proj.visibility = true;
        shooting = true;

        proj.vel.x = 0 + vel.x*.2;
        proj.vel.y = PROJECTILE_SPEED + vel.y*.2;
    }
}

void Player::updatePhysics() {


    // Player random walk since input doesnt work 
    if ((getFrame() % 50 )== 0) {
        static int angle=0;
        angle++;

        vel.x += PLAYER_SPEED*cos(angle);
        vel.y += PLAYER_SPEED*sin(angle);
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
