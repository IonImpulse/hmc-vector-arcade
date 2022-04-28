#include <string>
#include <bitset>
#include <cmath>
#include <vector>

#include "../include/scene.h"
#include "../include/enemy.h"
#include "../include/basics.h"
#include "../include/player.h"

#include "../include/rawio.h"
// Debug
//#include "../include/stm32_helper.h"



///////////////////
///////////////


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


void updateMoveVector(Player* player) {
   if (M_UP) {
       player->vel.y += ACCELERATION*M_UP; 
       M_UP *= .5;
   }
   if (M_DOWN) {
       player->vel.y -= ACCELERATION*M_DOWN;
       M_DOWN *= .5; 
   }
   if (M_LEFT) {
       player->vel.x -= ACCELERATION*M_LEFT;
       M_LEFT *= .5; 
   }
   if (M_RIGHT) {
       player->vel.x += ACCELERATION*M_RIGHT;
       M_RIGHT  *= .5; 
   }
   if (player->vel.x > PLAYER_SPEED){
       player->vel.x = PLAYER_SPEED;
   }
    if (player->vel.y > PLAYER_SPEED){
       player->vel.y = PLAYER_SPEED;
   }
}
   

void handlePlayerProj(Player* player) { 
     if (M_shoot && !player->shooting) {
        player->shoot();
    }


    if(player->shooting) {
        
        M_shoot = false;
         // std::cerr << "I was hit" << std::endl;
    }
}

void doNextFrame() {    
    
 
    doAllProjectiles();
    doAllEntities();

    draw_absolute_vector(-512,0,0);
    draw_relative_vector(1023,0,255);

    draw_end_buffer();
}


int main() {
    initialize_input_output();
    sendString("Welcome to HMC Vector Arcade!\n\r");

    Player player = Player(0,-120,PLAYER_SIZE,"player");
    addEntity(&player); // player must be added first 
    addProjectile(&player.proj);

    //spawnEnemy(12,10,20,"basic");
    spawnEnemy(-4,10,20,"basic");
    spawnEnemy(10,5,20,"basic");


    


    // Render loop
    
    while (1) {
        start_timer(FRAME_DELAY_MS);
        takeInput();
        handlePlayerProj(&player);

        updateMoveVector(&player);
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
    deleteAll();
}
