#include <string>
#include <iostream>

#include "include/basics.h"

object2D::object2D(float startx, float starty, float size)  
    : SIZE{size} { 
        pos.x =startx;
    pos.y = starty;
}
object2D::object2D() {
    Vec2 pos = {1,2};
    Vec2 vel = {0,0}; 
    float ACCELERATION = 1; 
    float SIZE = 20;
}

void object2D::drawObject() {
    draw_absolute_vector(pos.x, pos.y, 0);
    draw_absolute_vector(pos.x + SIZE, pos.y, 1023);
    draw_absolute_vector(pos.x + SIZE, pos.y + SIZE, 1023);
    draw_absolute_vector(pos.x, pos.y + SIZE, 1023);
    draw_absolute_vector(pos.x, pos.y, 1023);
    draw_absolute_vector(-250,0,0);
    draw_absolute_vector(250,0,255);
}
void object2D::handleCollision() {
    std::cerr <<  name;
    std::cerr <<  " was hit" << std::endl;
}
