#include <iostream>
#include <unistd.h>
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

    addEntity(&player);
    addProjectile(&(player.proj));
    addEntity(&baddie);
    initialize_input_output();

    // Render loop
    while (1) {
        set_sleep_time_ms(FRAME_DELAY_MS);

        takeInput();
        updateMoveVector();
        updatePhysics();
        checkAllCollisions();
        doNextFrame();
        updateTimer();

        if (!sleep_until_set_time()) {
            std::cerr<<"Frame computation too long!"<<std::endl;
        }
    }
}
