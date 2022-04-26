#ifndef SCENE
#define SCENE
#include "basics.h"
#include <unordered_map>
#include <vector>
#include "projectile.h"



struct Scene {
        //16 x 16 ,  -8 -> 8 (256/32)


        object2D* entities[SCENE_SIZE];
        int entIn = 0 ;
        bool entFull = false;
        int currentFrame = 0;
        int currentTime = 0;

        Projectile* projectiles[SCENE_SIZE*3];
        int projIn = 0 ;
        bool projFull = false;
         //3 bullets per entity. 
        // std::unordered_map<std::pair<int,int>, Projectile> projectiles;
        unsigned int level = 0;

};

void spawnEnemy(int startx,int starty, int startsize, std::string name);

void addEntity(object2D* entity);

void doAllProjectiles();

void doAllEntities();

void addProjectile(Projectile* proj);

bool checkCollision(object2D* entity, object2D* proj );

void updateTimer();

void  checkAllCollisions();

void deleteAll();

bool everyX(int x);


#endif
