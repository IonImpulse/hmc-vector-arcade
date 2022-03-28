#include <iostream>
#include <chrono>
#include <unistd.h>
#include <string>
#include <bitset>
#include "enemy.h"
#include "basics.h"
#include "player.h"

#include "rawio.h"

#define FRAME_DELAY .03



// using namespace System;
using namespace std;

object2D* scene[20];

Player player(0,0,20);



// Movement:
float M_LEFT = 0;
float M_UP = 0;
float M_RIGHT = 0;
float M_DOWN = 0;
 
float ACCELERATION = 6; 

void takeInput() {
    InputState input = get_inputs();
    if (input.ypos < 0) {
        M_DOWN = 1;
    } else if (input.ypos > 0) {
        M_UP = 1;
    }
    if (input.xpos < 0) {
        M_LEFT = 1;
    } else if (input.xpos > 0) {
        M_RIGHT = 1;
    }
}

void updateMoveVector() {
   if (M_UP) {
       player.vel.y += ACCELERATION*M_UP; 
       M_UP *= .5;
   }
   if (M_DOWN) {
       player.vel.y -= ACCELERATION*M_DOWN;
       M_DOWN *= .5; 
   }
   if (M_LEFT) {
       player.vel.x -= ACCELERATION*M_LEFT;
       M_LEFT *= .5; 
   }
   if (M_RIGHT) {
       player.vel.x += ACCELERATION*M_RIGHT;
       M_RIGHT  *= .5; 
   }
}
   
void updatePhysics() {
    // player 
    player.pos.x += player.vel.x;
    player.pos.y += player.vel.y;

    if(player.pos.x < -256) {
        player.pos.x = -256;
        player.vel.x = 0;
    }
    if((player.pos.x + player.SIZE) > 256) {
        player.pos.x = 256 - player.SIZE;
        player.vel.x = 0;
    }
    if(player.pos.y < -256) {
        player.pos.y = -256;
        player.vel.y = 0;
    }
    if((player.pos.y + player.SIZE) > 0) {
        player.pos.y = 0 - player.SIZE;
        player.vel.y = 0;
    }

    player.vel.x *= .9;
    player.vel.y *= .9;


}


void doNextFrame() {
    Vec2 point1 = {0,40};
    Vec2 point2 = {-100,150};
    Vec2 point3 = {0,50};
    Vec2 point4 = {100,256};
    Vec2 path[4] = {point1, point2, point3, point4};
    Enemy baddie = Enemy(0,30,path);

    draw_buffer_switch();  
    
    player.drawObject();
    baddie.drawEnemy();
    baddie.updateEnemy();

    draw_end_buffer();
    
}

int main() {
    
    // Triangle tris[] {
    //     Triangle{Coord{0, 0, 0}, Coord{1, 0, 0}, Coord{0, 1, 0}}, 
    //     Triangle{Coord{1, 0, 0}, Coord{1, 1, 0}, Coord{0,1,0}}
    // };
    initialize_input_output();
    
    // Render loop
    typedef std::chrono::high_resolution_clock Clock;
    while (1) {
        auto start = Clock::now();
        doNextFrame();
        auto end = Clock::now();

        takeInput();
        updateMoveVector();

        std::chrono::duration<double> frameTimeObj = end - start;
        double frameTime = frameTimeObj.count();
        if (frameTime > FRAME_DELAY) {
            std::cerr<<"Frame computation too long!"<<std::endl;
        }
        else {
            unsigned int sleep_dur_us = (FRAME_DELAY-frameTime)*1e6;
            // std::cout << sleep_dur_us << std::endl;
            usleep(sleep_dur_us);
        }
    }
}
