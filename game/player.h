#ifndef PLAYER
#define PLAYER
#include "basics.h"
#include <string>

class Player : public object2D
{
    public:
        //int team = 1;

    using object2D::object2D;

    void drawPlayer() {
        object2D::drawObject();
    }
    
    
};



#endif