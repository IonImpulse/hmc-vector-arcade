#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <unistd.h>
#include <chrono>

#include "util.hpp"

#define FRAME_DELAY 1
#define SCREEN_HEIGHT 2400
#define SCREEN_WIDTH 2400
#define PI 3.141592f

// Important Constants

const float CLOSE_PLANE = 0.1f;
const float FAR_PLANE = 1000.0f;
const float FOV = 90; // In degrees?
const float ASP = SCREEN_HEIGHT/SCREEN_WIDTH;

std::string FILENAMES = {"buff0", "buff1"};

uint8_t current_buffer = 0;
game_object objects[255];
std::ofstream* buffer_handle;

void draw_triangle(triangle* tri) {
    std::ofstream output ("buff0", std::ios::out);
    
    for (auto &point : tri -> points) {
        output << "vec " << point.x <<  ' ' << point.y << ' ' << 255 << '\n'; 
    }
    output << "halt";
}

void render_frame() {
    while(1){
        std::cout << 0 << '\n';
    }
}

int main(int argc, char** argv) {
    game_object cube;
    cube.tris = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		};

    triangle t = {
        vec3{0.0f, 0.0f, 0.0f}, vec3{1000.0f, 0.0f, 0.0f}, vec3{0.0f, 100.0f, 0.0f}
    };

    draw_triangle(&t);

    typedef std::chrono::high_resolution_clock Clock;
    while(1) {
        auto start = Clock::now();
        render_frame();
        auto end = Clock::now();
        std::chrono::duration<float> frame_time_counter = end - start;
        float frame_time = frame_time_counter.count();
    
        sleep(1);
    }

    return 0;
}
