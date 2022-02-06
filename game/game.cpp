#include <iostream>
#include <chrono>
#include <unistd.h>
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
    static int buffer_select = 0;
    buffer_select ^= 1;
    std::cout << buffer_select << std::endl;
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
            //std::cout << sleep_dur_us << std::endl;
            usleep(sleep_dur_us);
        }
    }
}
