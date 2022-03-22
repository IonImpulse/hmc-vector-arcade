#include <iostream>
#include <chrono>
#include <unistd.h>
#include <string>
#include <bitset>
#include "wrapper.cpp"
#include <conio.h>


#define FRAME_DELAY .01

// using namespace System;
using namespace std;



int box_x = 0;
int box_y = 0;


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

    draw_buffer_switch();  
    absolute_vec(box_x, box_y, 0);
    absolute_vec(box_x + 100, box_y, 1023);
    absolute_vec(box_x + 100, box_y + 100, 1023);
    absolute_vec(box_x, box_y + 100, 1023);
    absolute_vec(box_x, box_y, 1023);
    absolute_vec(-250,-10,0);
    absolute_vec(250,1-10,255);

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

       

        if(kbhit())
        {
            int input = _getch();
            if (input == 100){ //pressed d 
                box_x = box_x + 10;
            }
             if (input == 115){ //pressed s
                box_y = box_y - 10;
            }
             if (input == 119){ //pressed w
                box_y = box_y + 10;
            }
            if (input == 97){ //pressed a
                box_x = box_x - 10;
            }
            
           

            if (box_x > 256) {
                box_x = -356;
            }
        }
        

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
