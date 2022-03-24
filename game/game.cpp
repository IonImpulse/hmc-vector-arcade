#include <iostream>
#include <chrono>
#include <unistd.h>
#include <string>
#include <bitset>
#include "wrapper.cpp"
#include <conio.h>


#define FRAME_DELAY .03
#define PLAYER_SIZE 20

// using namespace System;
using namespace std;

typedef struct {
    float x, y, z, w;
} Coord;

typedef struct {
    float x = 0;
    float y = 0;
} Vec2;


typedef struct {
    Coord c1, c2, c3;
} Triangle;

typedef struct {
    Triangle* tri_list;
} Object;



Vec2 box;

// Movement:
float M_LEFT = 0;
float M_UP = 0;
float M_RIGHT = 0;
float M_DOWN = 0;
 Vec2 VELOCITY; 
float ACCELERATION = 6; 

void takeInput() {
    if(kbhit())
        {
            int input = _getch();
            if (input == 100){ //pressed d 
                M_RIGHT = 1; 
            }
             if (input == 115){ //pressed s
                M_DOWN = 1; 
            }
             if (input == 119){ //pressed w
                M_UP = 1;
            }
            if (input == 97){ //pressed a
               M_LEFT = 1;
            }
         
    } 
    

}

void updateMoveVector() {
   if (M_UP) {
       VELOCITY.y += ACCELERATION*M_UP; 
       M_UP *= .5;
   }
   if (M_DOWN) {
       VELOCITY.y -= ACCELERATION*M_DOWN;
       M_DOWN *= .5; 
   }
   if (M_LEFT) {
       VELOCITY.x -= ACCELERATION*M_LEFT;
       M_LEFT *= .5; 
   }
   if (M_RIGHT) {
       VELOCITY.x += ACCELERATION*M_RIGHT;
       M_RIGHT  *= .5; 
   }
   
}

void updatePhysics() {
    // player 
    box.x += VELOCITY.x;
    box.y += VELOCITY.y;

    if(box.x < -256) {
        box.x = -256;
    }
    if((box.x + PLAYER_SIZE) > 256) {
        box.x = 256 - PLAYER_SIZE;
    }
    if(box.y < -256) {
        box.y = -256;
    }
    if((box.y + PLAYER_SIZE) > 0) {
        box.y = 0 - PLAYER_SIZE;
    }

    VELOCITY.x *= .9;
    VELOCITY.y *= .9;


}

void doNextFrame() {

    draw_buffer_switch();  
    absolute_vec(box.x, box.y, 0);
    absolute_vec(box.x + PLAYER_SIZE, box.y, 1023);
    absolute_vec(box.x + PLAYER_SIZE, box.y + PLAYER_SIZE, 1023);
    absolute_vec(box.x, box.y + PLAYER_SIZE, 1023);
    absolute_vec(box.x, box.y, 1023);
    absolute_vec(-250,0,0);
    absolute_vec(250,0,255);

    halt();
    
}

int main() {
    
    Triangle tris[] {
        Triangle{Coord{0, 0, 0}, Coord{1, 0, 0}, Coord{0, 1, 0}}, 
        Triangle{Coord{1, 0, 0}, Coord{1, 1, 0}, Coord{0,1,0}}
    };


    // Render loop
    typedef std::chrono::high_resolution_clock Clock;
    while (1) {
        auto start = Clock::now();
        doNextFrame();
        auto end = Clock::now();

        takeInput();
        updateMoveVector();
        updatePhysics();


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
