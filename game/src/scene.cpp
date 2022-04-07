#include "include/scene.h"

struct Scene scene;

void addEntity(object2D* entity){
        scene.entities[scene.entIn] = entity;
        if(scene.entIn == SCENE_SIZE ){
           scene.entFull = true; 
        }
        scene.entIn = (scene.entIn + 1) % SCENE_SIZE;

        //project player entity
        if (scene.entIn == 0){
                scene.entIn++;
        }
}

void drawAllProjectiles() {
        int endIndex = scene.projIn;
        if (scene.projFull) {
                endIndex = SCENE_SIZE*3;
        }
        for (int i = 0; i < endIndex; i++) {
                scene.projectiles[i]->drawProj();
        }
}


void addProjectile(Projectile* proj){
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
    int ob2Hieght =proj->SIZE ;
        


    if (entity->pos.x < proj->pos.x + ob2Width &&
        entity->pos.x + ob1Width > proj->pos.x &&
        entity->pos.y < proj->pos.y + ob2Hieght &&
        entity->pos.y + ob1Hieght > proj->pos.y  ){

        entity->handleCollision();
        return true;     
       
      
    }
   
     return false; 

}
void updateTimer() {
        scene.currentFrame++;
        scene. currentTime += FRAME_DELAY;
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

