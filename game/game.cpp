#include <iostream>
#include <chrono>
#include <unistd.h>
#include <string>
#include <bitset>
#include <cmath>
#include "wrapper.cpp"
#include <conio.h>


#include "enemy.h"
#include "basics.h"
#include "player.h"
#include "scene.h"

#include <vector>

#define FRAME_DELAY .03



// using namespace System;




Player player(0,0,20);
Scene alpha(5); 
int x = alpha.index;
// alpha.addObject(object2D player);
// alpha.printAllx();

// Movement:
float M_LEFT = 0;
float M_UP = 0;
float M_RIGHT = 0;
float M_DOWN = 0;
bool M_shoot = false;
 
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
            if (input == 32){
                M_shoot = true;
            }
         
    } 
    

}
/////////////////////////////

////////////////////////////
bool checkCollision(object2D ob1, object2D ob2 ) {
    int ob1Width = ob2.SIZE;
    int ob1Hieght =ob2.SIZE ;
    
    int ob2Width = ob2.SIZE;
    int ob2Hieght =ob2.SIZE ;

    if (ob1.pos.x < ob2.pos.x + ob2Width &&
        ob1.pos.x + ob1Width > ob2.pos.x &&
        ob1.pos.y < ob2.pos.y + ob2Hieght &&
        ob1.pos.y + ob1Hieght > ob2.pos.y  ){

         std::cerr << "The enemy was hit" << std::endl;
        return true;     
       
      
    }
   
     return false; 

}
///////////////////////////

//////////////////////////


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
    player.updatePhysics();


}

void doNextFrame() {
    
    Enemy baddie = Enemy(0,30,30);

    
    if (M_shoot && !player.shooting) {
        player.shoot();

    }
    if(player.shooting) {
        player.proj.drawProj();
        M_shoot = false;
        checkCollision(player.proj, baddie);
    }
    

    draw_buffer_switch();  
    
    player.drawObject();
    baddie.drawEnemy();
    baddie.updateEnemy();

    halt();
    
}

int main() {
    
    // Triangle tris[] {
    //     Triangle{Coord{0, 0, 0}, Coord{1, 0, 0}, Coord{0, 1, 0}}, 
    //     Triangle{Coord{1, 0, 0}, Coord{1, 1, 0}, Coord{0,1,0}}
    // };

    
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
