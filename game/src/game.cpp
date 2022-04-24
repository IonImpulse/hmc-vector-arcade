#include <iostream> // <-- UHHH why does this even compile for the embedded system???
#include <unistd.h> // <-- UHHH why does this even compile for the embedded system???
#include <string>
#include <bitset>
#include <cmath>
#include <vector>

#include "../include/scene.h"
#include "../include/enemy.h"
#include "../include/basics.h"
#include "../include/player.h"

#include "../include/rawio.h"

#define PLAYER_SIZE 50


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
    if (input.buttons){
        M_shoot = true;
    }
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
    player.drawObject();
    baddie.drawEnemy();
    baddie.updateEnemy();
    handlePlayerProj();
    drawAllProjectiles();
    draw_end_buffer();
}

int main() {
    initialize_input_output();
    // for the time being, these add statements are screwing up the picture for some reason
    //addEntity(&player);
    //addProjectile(&(player.proj));
    //addEntity(&baddie);

    // Render loop
    while (1) {
        start_timer(5);//FRAME_DELAY_MS);
        takeInput();
        updateMoveVector();
        updatePhysics();
        checkAllCollisions();
        doNextFrame();
        updateTimer();
        request_halt(); // if draw time and compute time are comparable, consider moving this up above some of the computations
        // however if the computations are really long and the draw time is small, then you'd risk activating spot-killer
        while(!is_halted()) {}
        draw_buffer_switch(); // deactivates the halted state and halt request

        if (timer_done()) {
            sendString("Frame computation too long!");
        } else {
            // we don't want to be requesting halt right now
            // because the vector generator may need to draw multiple times
            // while waiting out the frame delay
            while (!timer_done()) {}
        }
    }
}
