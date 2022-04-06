#include <iostream>
#include <chrono>
#include <unistd.h>
#include <string>
#include <bitset>
<<<<<<< HEAD
#include <cmath>
#include "wrapper.cpp"
#include <conio.h>

#include "scene.h"
=======
>>>>>>> 59545079acc6b73c55c3559f0277c06a6838e1d0
#include "enemy.h"
#include "basics.h"
#include "player.h"

#include "rawio.h"

<<<<<<< HEAD
#include <vector>
=======
#define FRAME_DELAY .03
#define PLAYER_SIZE 50


>>>>>>> 59545079acc6b73c55c3559f0277c06a6838e1d0


<<<<<<< HEAD

=======
object2D* scene[20];

Player player(0,0,20);
>>>>>>> 59545079acc6b73c55c3559f0277c06a6838e1d0

///////////////////
///////////////

 Player player = Player(0,0,20,"player");
Enemy baddie = Enemy(20,30,30,"baddie");
//////////////
//////////////////

// Movement:
float M_LEFT = 0;
float M_UP = 0;
float M_RIGHT = 0;
float M_DOWN = 0;
bool M_shoot = false;
 
float ACCELERATION = 6; 


void takeInput() {
<<<<<<< HEAD
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
    

=======
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
>>>>>>> 59545079acc6b73c55c3559f0277c06a6838e1d0
}
/////////////////////////////

/////////////////////s///////

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

void handlePlayerProj() { 
     if (M_shoot && !player.shooting) {
        player.shoot();
    }


    if(player.shooting) {
        
        M_shoot = false;
         // std::cerr << "I was hit" << std::endl;
    }
}
void doNextFrame() {

    
   
    

    draw_buffer_switch();  
    
    player.drawObject();
    baddie.drawEnemy();
    baddie.updateEnemy();
    handlePlayerProj();

    drawAllProjectiles();
    


    draw_end_buffer();
    
}

int main() {
    
    // Triangle tris[] {
    //     Triangle{Coord{0, 0, 0}, Coord{1, 0, 0}, Coord{0, 1, 0}}, 
    //     Triangle{Coord{1, 0, 0}, Coord{1, 1, 0}, Coord{0,1,0}}
    // };
<<<<<<< HEAD

    addEntity(&player);
    addProjectile(&(player.proj));
    addEntity(&baddie);


    // Render loop
    typedef std::chrono::high_resolution_clock Clock;
    while (1) {
        
        


        auto start = Clock::now();
        takeInput();
        updateMoveVector();
        updatePhysics();
        checkAllCollisions();
        doNextFrame();
        updateTimer();
        auto end = Clock::now();
  



=======
    initialize_input_output();
    
    // Render loop
    typedef std::chrono::high_resolution_clock Clock;
    while (1) {
        auto start = Clock::now();
        doNextFrame();
        updateMoveVector();
        updatePhysics();
>>>>>>> 59545079acc6b73c55c3559f0277c06a6838e1d0

        auto end = Clock::now();

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
