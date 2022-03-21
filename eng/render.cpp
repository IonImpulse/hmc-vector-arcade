#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <unistd.h>
#include <chrono>
#include <Terathon-Math-Library/TSVector3D.h>
#include <Terathon-Math-Library/TSVector4D.h>
#include <Terathon-Math-Library/TSBivector4D.h>
#include <Terathon-Math-Library/TSTrivector4D.h>
#include <Terathon-Math-Library/TSMotor4D.h>
#include "util.hpp"

#define FRAME_DELAY 1
#define SCREEN_HEIGHT 2400
#define SCREEN_WIDTH 2400

std::string FILENAMES = {"buff0", "buff1"};

uint8_t current_buffer = 0;
game_object objects[255];
std::ofstream* buffer_handle;

void draw_triangle(triangle& tri) {
    std::ofstream buffer;
    std::string filename = (current_buffer)? "buff1":"buff0";
    buffer.open(filename);

    for (auto vec: tri.points) {
        buffer << "vec "<< vec.x << ' ' << vec.y << ' ' << 255 << '\n';
    }
    buffer << "vec " << tri.points[0].x << ' ' << tri.points[0].y << ' ' << 255 << '\n';
    buffer << "halt\n";

    buffer.close();
}



void render_frame() {
    current_buffer = (current_buffer + 1) % 2;
    
    std::string filename = (current_buffer)? "buff1" : "buff0";
    std::ofstream buff;
    buffer_handle = &buff;
    
    //buff.open(filename, std::ofstream::out | std::ofstream::trunc); // Clear buffer for writing

    for (game_object g: objects) {
        for (triangle t: g.tris) {
            draw_triangle(t); 
        }
    }
        
    buff.close();
    std::cout << current_buffer << std::endl;
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
        vec3{0.0f, 0.0f, 0.0f}, vec3{2048.0f, 0.0f, 0.0f}, vec3{0.0f, 100.0f, 0.0f}
    };

    draw_triangle(t);

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
