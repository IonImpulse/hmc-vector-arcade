#include <iostream>
#include <chrono>
#include <unistd.h>
#include <string>
#include <bitset>

#define FRAME_DELAY .3
using namespace std;

int current  = 0;

void doNextFrame() {
    current = (current + 1)%2;
    cout << current << endl;
}

int main() {


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
