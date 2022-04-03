#include "enemy.h"

Enemy::Enemy(float startx, float starty, Vec2 inPath[4]) {
    path[4] = *inPath;
    Velocity.x = (startx-inPath[0].x)*.1;
    Velocity.y = (starty-inPath[0].y)*.1;
    x = startx;
    y = starty;
}

void Enemy::drawEnemy() {
    draw_absolute_vector(x, y, 0);
    draw_relative_vector(ENEMY_SIZE, 0, 900);
    draw_relative_vector(-(ENEMY_SIZE/2), ENEMY_SIZE, 1023);
    draw_absolute_vector(x, y, 1023);   
}
void Enemy::updateEnemy() {
    x += Velocity.x; 
    y += Velocity.y;
}
