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
float M_X = 0;
float M_Y = 0;
bool M_shoot = true;
 
float ACCELERATION = 0.2; 


void takeInput() {
    InputState input = get_inputs();
    
    M_X = (input.xpos-0.5);
    M_Y = (input.ypos-0.5);
    if (input.buttons){
        M_shoot = true;
    }
}
/////////////////////////////

/////////////////////s///////

///////////////////////////

//////////////////////////


void updateMoveVector(Player* player) {
   player->vel.x += ACCELERATION*M_X;
   player->vel.y += ACCELERATION*M_Y;

   if (player->vel.x > PLAYER_SPEED){
       player->vel.x = PLAYER_SPEED;
   } else if (player->vel.x < -PLAYER_SPEED){
       player->vel.x = -PLAYER_SPEED;
   }
   if (player->vel.y > PLAYER_SPEED){
       player->vel.y = PLAYER_SPEED;
   } else if (player->vel.y < -PLAYER_SPEED){
       player->vel.y = -PLAYER_SPEED;
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
    spawnEnemy(-4,10,40,"basic");
    spawnEnemy(10,5,40,"basic");


    


    // Render loop
    
    while (1) {
        start_timer(FRAME_DELAY_MS);
        handleLevel();

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
