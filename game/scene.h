#ifndef SCENE
#define SCENE
#include "basics.h"
#include <unordered_map>

#define SCENE_SIZE 20



class Scene {
    public: 
        // 16 x 16 ,  -4 -> 4 
        //std::unordered_map<std::pair<int,int>, int> umap;
        int index=0; 

    Scene(int in) {
        //std::pair<int,int> firstVal (1,1);
        // umap[firstVal] = 10;
        index = in;
    }

    void printAllx() {
        std::cerr<<"All x positions"<<std::endl;

       for(int i = 0; i < index; i++) {
          // std::cerr<<   space[i].pos.x <<std::endl;
          
       }

    }
     void addObject(object2D object) {
       //space[index] = object; 
       ++index ;
    }

};
#endif