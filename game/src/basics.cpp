#include <string>
#include <iostream>


#include "../include/basics.h"

object2D::object2D(float startx, float starty, float size, std::string inName)  
    : SIZE{size} { 
        name = inName;
        pos.x =startx;
    pos.y = starty;
}
object2D::object2D() {
    pos = {1,2};
    vel = {0,0}; 
    ACCELERATION = 1; 
    SIZE = 20;
}
object2D::~object2D(){}


void object2D::updatePhysics() { 
    return;
}


void object2D::drawObject() {
    draw_absolute_vector(pos.x, pos.y, 0);
    draw_absolute_vector(pos.x + SIZE, pos.y, 1023);
    draw_absolute_vector(pos.x + SIZE, pos.y + SIZE, 1023);
    draw_absolute_vector(pos.x, pos.y + SIZE, 1023);
    draw_absolute_vector(pos.x, pos.y, 1023);
    
}
void object2D::handleCollision() {
    hitReact();
    std::cerr <<  name;
    std::cerr <<  " was hit" << std::endl;
}

void object2D::hitReact() {
    
}
