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
        unsigned long long int currentFrame = 0;
        unsigned long long int currentTime = 0;

        Projectile* projectiles[SCENE_SIZE*3];
        int projIn = 0 ;
        bool projFull = false;
         //3 bullets per entity. 
        // std::unordered_map<std::pair<int,int>, Projectile> projectiles;
        int level = 0;

           // -1 gameover 
        // 0 start screen 
        //
        int enemies = 0;
        bool gameOver = false;
        bool nextSceneAnimation = false; 
        unsigned long long int nextSceneTrigger = 0;
        unsigned long long int gameOverTrigger = 0;

     

};
void gameOver(); 

unsigned int getLevel() ;

void killEnemy() ;

unsigned long long int getFrame();

void spawnEnemy(int startx,int starty, int startsize, std::string name);

void addEntity(object2D* entity);

void handleLevel();

void doAllProjectiles();

void doAllEntities();

void addProjectile(Projectile* proj);

bool checkCollision(object2D* entity, object2D* proj );

void updateTimer();

void  checkAllCollisions();

void deleteAll();

void deleteEnemies();

bool everyX(unsigned long long int x);

void hideAll(); 

void initializeNextScene();

#endif
