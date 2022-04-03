#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <unistd.h>
#include <chrono>
#include "util.hpp"
#include <sstream>
#include "rawio.h"

#define FRAME_DELAY 1
#define SCREEN_HEIGHT 2400
#define SCREEN_WIDTH 2400
#define PI 3.141592f

/*
 * TODO
 *
 * Change to points and lines
 * Implement screen clipping
 * Make the darn thing work with the vector simulator
 * DEMO!
*/


// Important Constants

const float CLOSE_PLANE = 0.1f;
const float FAR_PLANE = 1000.0f;
const float FOV = 90; // In degrees?
const float ASP = SCREEN_HEIGHT/SCREEN_WIDTH;

int main(int argc, char** argv) {
 

    typedef std::chrono::high_resolution_clock Clock;
    while(1) {
        auto start = Clock::now();
        auto end = Clock::now();
        draw_absolute_vector(0,0,255);
        draw_relative_vector(100,0, 255);
        draw_relative_vector(0, 100, 255);
        draw_relative_vector(-100, 0, 255);
        draw_relative_vector(0, -100, 255);
        draw_end_buffer();
        draw_buffer_switch();
    
        sleep(1);
    }

    return 0;
}
