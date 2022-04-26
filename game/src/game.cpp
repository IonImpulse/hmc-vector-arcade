#include <string>
#include <bitset>
#include <cmath>
#include <vector>

#include "../include/scene.h"
#include "../include/enemy.h"
#include "../include/basics.h"
#include "../include/player.h"

#include "../include/rawio.h"



///////////////////
///////////////

Player player = Player(0,0,PLAYER_SIZE,"player");

Enemy baddie = Enemy(12,10,20,"baddie");
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
    
   
    handlePlayerProj();
    doAllProjectiles();
    doAllEntities();

    draw_absolute_vector(LEFT_X,SPLIT ,0);
    draw_absolute_vector(RIGHT_X,SPLIT,255);

    draw_end_buffer();
}





int main() {
    initialize_input_output();
    sendString("Welcome to HMC Vector Arcade!\n\r");

    addEntity(&player);
    addEntity(&baddie);
    addProjectile(&baddie.proj);


    // Render loop
    while (1) {
        start_timer(FRAME_DELAY_MS);
        takeInput();
        updateMoveVector();
        updatePhysics();
        checkAllCollisions();
        doNextFrame();
        updateTimer();
        while(!is_halted()) {} // wait until frame has finished drawing, if it hasn't already
        draw_buffer_switch(); // deactivates the halted state

        if (timer_done()) {
            sendString("Frame computation too long!\n\r");
        } else {
            while (!timer_done()) {}
        }
    }
}
