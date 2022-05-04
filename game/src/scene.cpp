#include "../include/scene.h"
#include <iostream>
#include "../include/enemy.h"
#include "../include/rawio.h"
#include "../include/render.h"


struct Scene scene;

void handleLevel() {
         // Ok so this checks if the victory condition is complete and moves to the next level. 
        // for all but start and gameover this is all the enemies are dead
        std::cerr << scene.enemies;
         std::cerr << " ";
         std::cerr << scene.level;
                  std::cerr << " ";


        
        
        if(scene.level == 0 ) {
                hideAll();
                
                //render title 
                renderTitle(-450,200,9);
                // tick down enemies as a clock system before level 1,

                if(getFrame() % 5 == 0) {
                        static int i = 0;
                        i+=1; 
                        if( i > FPS/5) {
                                i = 0;
                                scene.level += 1;
                                initializeNextScene();
                        }
                }
                // have button press render to start. 

        }  else if (scene.level == -1) { 
                hideAll();
                renderGameOver(-450, 200,9);

                if(getFrame() % 50 == 0) {
                        
                        static int i = 0;
                        i+=1; 
                        if( i > 5) {
                                i = 0;
                                scene.level += 1;
                                 initializeNextScene();

                        }
                }
                


        } else {
                        // normal level time: 
                renderHUD(scene.entities[0]->life );
                
                if (scene.enemies == 0 && !scene.nextSceneAnimation) {
                        scene.nextSceneAnimation = true; 
                        scene.nextSceneTrigger = getFrame() + FPS;//*FPS;
                }
                if (scene.nextSceneAnimation) {
                        //run annimation whipe
                }
                if (scene.nextSceneTrigger == getFrame() ) {
                        scene.level += 1;
                        scene.nextSceneAnimation = false; 
                        initializeNextScene();

                }

                ///r GAME OVER 
                if (scene.gameOver ) {
                        scene.gameOverTrigger = getFrame() + FPS;
                        hideAll();
                        //run annimation whipe
                        scene.gameOver = false;
                } if (scene.gameOverTrigger == getFrame()) {
                        scene.level = -1;
                }

                
                
        }

}

void gameOver(){ 
        sendString("Game Over");
        requestChiptune(song,0);
        scene.gameOver = true;
        
} 

unsigned int getLevel() { return scene.level; }

void killEnemy() { scene.enemies-=1; }


void addEntity(object2D* entity){
        delete scene.entities[scene.entIn];
        scene.entities[scene.entIn] = entity;
        if(scene.entIn == SCENE_SIZE ){
           scene.entFull = true; 
        }
        scene.entIn = (scene.entIn + 1) % SCENE_SIZE;

        //protect player entity
        if (scene.entIn == 0){
                scene.entIn++;
        }
        
        
}

void spawnEnemy(int startx,int starty,  int startsize, std::string name) {
        Enemy * p1 = new Enemy(startx,starty,startsize,name);
        addEntity(p1);
        addProjectile(&(p1->proj));
        scene.enemies += 1;
}



void doAllProjectiles() {

        int endIndex = scene.projIn;
        if (scene.projFull) {
                endIndex = SCENE_SIZE*3;
        }

        for (int i = 0; i < endIndex; i++) {
                if (scene.projectiles[i]->visibility) {
                        scene.projectiles[i]->drawProj();
                        scene.projectiles[i]->updatePhysics();
                }
        }
}

unsigned long long int getFrame() {
        return scene.currentFrame; 
}

void doAllEntities() {

        int endIndex = scene.entIn;
        if (scene.entFull) {
                endIndex = SCENE_SIZE*3;
        }
        for (int i = 0; i < endIndex; i++) {
                if (scene.entities[i]->visibility) {
                        scene.entities[i]->drawObject();
                        scene.entities[i]->updatePhysics();  
                }
        }
}



void addProjectile(Projectile* proj){
        delete scene.projectiles[scene.projIn];
        scene.projectiles[scene.projIn] = proj;
        if(scene.projIn == (SCENE_SIZE *3)){
           scene.projFull = true; 
        }
        scene.projIn = (scene.projIn + 1) % (SCENE_SIZE*3);

        //project player entity
}




bool checkCollision(object2D* entity, object2D* proj ) {
    int ob1Width = entity->SIZE;
    int ob1Hieght = entity->SIZE ;
    
    int ob2Width = proj->SIZE;
    int ob2Hieght =proj->SIZE + proj->vel.y/2;
    // The proj.vel.y addition is to offset the fact the projects move vertically verry fast 
    // and could jump over and enemy. 


        if(entity->visibility && proj->visibility) {
                if (entity->pos.x < proj->pos.x + ob2Width &&
                        entity->pos.x + ob1Width > proj->pos.x &&
                         entity->pos.y < proj->pos.y + ob2Hieght &&
                        entity->pos.y + ob1Hieght > proj->pos.y  ){
        
                        entity->handleCollision();
                        return true;     
         }
        }
   
     return false; 

}
void updateTimer() {
        scene.currentFrame++;
        scene.currentTime += FRAME_DELAY;
}

bool everyX(unsigned long long int x) {
        return ((scene.currentFrame % x) == 0);
}

void  checkAllCollisions() { 
        int entities;
        if (!scene.entFull) {
                entities = scene.entIn;
        } else {
                entities = SCENE_SIZE;
        }
         int projs;
        if (!scene.projFull) {
                projs = scene.projIn;
        } else {
                projs = SCENE_SIZE*3;
        }


        for (int e = 0; e < entities; e++) {
                for (int p = 0; p < projs; p++) {
                
                   checkCollision(scene.entities[e], scene.projectiles[p]);
                }
        }

}


//NOT USED 
void deleteAll() {
        int entities;
        if (!scene.entFull) {
                entities = scene.entIn;
        } else {
                entities = SCENE_SIZE;
        }

        for (int i = 1; i < entities; i++) {
                delete scene.entities[i];
        }
        // for (int i = 0; i < projs*3; i++) {
        //         delete scene.projectiles[i];
        // }
}

void deleteEnemies() {
        // saves the player 
        int entities;
        if (!scene.entFull) {
                entities = scene.entIn;
        } else {
                entities = SCENE_SIZE;
        }

        for (int i = 1; i < entities; i++) {
                delete scene.entities[i];
        }
        scene.entIn = 1; 
        scene.entFull = false;
        
        scene.projIn = 1; 
        scene.projFull = false;

       
}

void hideAll(){ 
        int entities;
        if (!scene.entFull) {
                entities = scene.entIn;
        } else {
                entities = SCENE_SIZE;
        }
         int projs;
        if (!scene.projFull) {
                projs = scene.projIn;
        } else {
                projs = SCENE_SIZE*3;
        }


        for (int e = 0; e < entities; e++) {
                scene.entities[e]->visibility = false;
        }
        for (int p = 0; p < projs; p++) {
                scene.projectiles[p]->visibility = false;
        }

}       

void initializeNextScene() { 
        //deleteEnemies();
        object2D* player = scene.entities[0] ;
        player->pos.x = 0;
        player->pos.y = -100;
        player->vel.x = 0;
        player->vel.y = 0;
        player->visibility = true;
        
      
        scene.enemies = 0; 
        for (int i = 0; i < scene.level+1; i++) {
                int left = 1;
                if(i % 2 == 1) {
                        left = -1;
                } 
                spawnEnemy( (i*10+4)*left, i*10+4, ENEMY_SIZE, "baddie");
        }

}
