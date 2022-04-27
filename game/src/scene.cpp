#include "../include/scene.h"
#include <iostream>
#include "../include/enemy.h"


struct Scene scene;

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
    int ob2Hieght =proj->SIZE ;
        

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

void deleteAll() {
        for (int i = 0; i < SCENE_SIZE; i++) {
                delete scene.entities[i];
        }
        for (int i = 0; i < SCENE_SIZE*3; i++) {
                delete scene.projectiles[i];
        }
}

