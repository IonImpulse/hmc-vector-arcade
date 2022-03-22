#include <iostream>
#include <chrono>
#include <unistd.h>
#include <string>
#include <bitset>
#include "wrapper.cpp"

#define FRAME_DELAY .3



typedef struct {
    float x, y, z, w;
} Coord;

typedef struct {
    Coord c1, c2, c3;
} Triangle;

typedef struct {
    Triangle* tri_list;
} Object;

void doNextFrame() {
    int box_x = 0;
    int box_y = 0;
    draw_buffer_switch();  
    absolute_vec(box_x, box_y, 0);
    absolute_vec(box_x + 100, box_y, 1023);
    absolute_vec(box_x + 100, box_y + 100, 1023);
    absolute_vec(box_x, box_y + 100, 1023);
    absolute_vec(box_x, box_y, 1023);
    halt();
    // std::cout << "0111"  << std::endl;
    // std::cout << "10000000000000000000000000000000"  << std::endl;
    // std::cout << "10000110010000000000001111111111"  << std::endl;

    // std::cout << "0"  << std::endl;
}

int main() {
    
    Triangle tris[] {
        Triangle{Coord{0, 0, 0}, Coord{1, 0, 0}, Coord{0, 1, 0}}, 
        Triangle{Coord{1, 0, 0}, Coord{1, 1, 0}, Coord{0,1,0}}
    };


    // Render loop
    typedef std::chrono::high_resolution_clock Clock;
    while (1) {
        auto start = Clock::now();
        doNextFrame();
        auto end = Clock::now();
        std::chrono::duration<double> frameTimeObj = end - start;
        double frameTime = frameTimeObj.count();
        if (frameTime > FRAME_DELAY) {
            std::cerr<<"Frame computation too long!"<<std::endl;
        }
        else {
            unsigned int sleep_dur_us = (FRAME_DELAY-frameTime)*1e6;
            // std::cout << sleep_dur_us << std::endl;
            usleep(sleep_dur_us);
        }
    }
}
