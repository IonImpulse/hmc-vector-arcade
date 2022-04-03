#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <unistd.h>
#include <chrono>
#include "util.hpp"
#include <sstream>

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

std::string FILENAMES = {"buff0", "buff1"};

uint8_t current_buffer = 0;
game_object objects[255];
std::ofstream* buffer_handle;

std::string draw_triangle(triangle& tri) {
    std::stringstream s("");
    vec3* points = tri.points;

    s << "vec " << points[1].x - points[0].x << ' ' << points[1].y - points[0].y << ' ' << 255 << '\n';
    s << "vec " << points[2].x - points[1].x << ' ' << points[2].y - points[1].y << ' ' << 255 << '\n';
    s << "vec " << points[0].x - points[2].x << ' ' << points[0].y - points[2].y << ' ' << 255 << '\n';

    return s.str(); 
}

void render_frame(std::vector<game_object>& objects) {
    std::ofstream OUT("buff0", std::ios::out);
    
    for (game_object& object : objects) {
        for(triangle& tri: object.tris) {
            std::string mapping = draw_triangle(tri);
            OUT << mapping;
        }
    }

    OUT << "halt\n";
    OUT.close();
    
    std::cout << "0\n";
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
        vec3{0.0f, 0.0f, 0.0f}, vec3{200.0f, 0.0f, 0.0f}, vec3{0.0f, 100.0f, 0.0f}
    };

    game_object g;
    std::vector<triangle> tris; tris.push_back(t);
    g.tris = tris;
    std::vector<game_object> gs; gs.push_back(g);

    typedef std::chrono::high_resolution_clock Clock;
    while(1) {
        auto start = Clock::now();
        render_frame(gs);
        auto end = Clock::now();
        std::chrono::duration<float> frame_time_counter = end - start;
        float frame_time = frame_time_counter.count();
    
        sleep(1);
    }

    return 0;
}
