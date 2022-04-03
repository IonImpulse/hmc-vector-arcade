#include "basics.h"

object2D::object2D(float startx, float starty, float size)  
    : SIZE{size} { 
        pos.x =startx;
    pos.y = starty;
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

