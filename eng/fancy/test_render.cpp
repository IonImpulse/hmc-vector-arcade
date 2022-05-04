#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <unistd.h>
#include <chrono>
#include "pga.hpp"
#include "util.hpp"
#include "entity.hpp"

#define FRAME_DELAY 1
#define SCREEN_HEIGHT 2400
#define SCREEN_WIDTH 2400

using namespace Terathon;

std::string FILENAMES = {"buff0", "buff1"};

uint8_t current_buffer = 0;
game_object objects[255];
std::ofstream* buffer_handle;


/*
 * The axes are oriented so that x is left and right, y is up and down, and z is forwards and backwards
 * Because of this orientation, positive x point left in stead of right and so the axes of look like this
 *                                       ^      
 *                                    +y |    /
 *                                       |   / +z <- Front of ship points in this direction
 *                                       |  /
 *                                       | / 
 *                         +x            |/           -x
 *                       <---------------|--------------->
 *                                      /|
 *                                     / |
 *                                    /  |
 *                                -z /   |
 *                                  /    | -y 
 *                                       V
 *
 *
 *
 * The plane of projection is perpendicular to the x and y axes of the ship's frame and one unit in front of the ship's origin
 */ 

#define X_MIN -512
#define X_MAX 511
#define Y_MIN -512
#define Y_MAX 512
const Point3D Zero3D = Point3D{0,0,0};

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

Point3D convertToShipCoords(const Point3D& point, const Ship& ship) {
    Vector3D relativePos = point - ship->origin;
    float x = Dot(relativePos, ship->x);
    float y = Dot(relativePos, ship->y);
    float z = Dot(relativePos, ship->z);
    return Point3D{x, y, z};
}


// Source: https://ami.uni-eszterhazy.hu/uploads/papers/finalpdf/AMI_52_from199to215.pdf
char endPointCode(const Point3D& p) {
    return  (p[0] < X_MIN*p[2])       + ((p[0] > X_MAX*p[2]) << 1) +
           ((p[1] < Y_MIN*p[2]) << 2) + ((p[1] > Y_MAX*p[2]) << 3);    
}

char entityInFrame(const Point3D& p, const float radius) {
    return  (p[0] + radius < X_MIN*p[2])       + ((p[0] - radius > X_MAX*p[2]) << 1) + 
           ((p[1] + radius < Y_MIN*p[2]) << 2) + ((p[1] - radius < Y_MAX*p[2]) << 3);
}

void printLine(const ofstream& out, const Point2D& a, const Point2D& b, int brightness) {
    buff << a[0] << "," << a[1] << "," 
         << b[0] << "," << b[1] << "," 
         << brightness << std::endl;

}

void render_frame(const std::vector<Entity>& entities, const Ship& ship) {
    current_buffer = (current_buffer + 1) % 2;
    
    std::string filename = (current_buffer)? "buff1" : "buff0";
    std::ofstream buff;
    buffer_handle = &buff;
    
    //buff.open(filename, std::ofstream::out | std::ofstream::trunc); // Clear buffer for writing
    for (Entity e: entities) {
        Model& model = e.model;
        Point3D relPos = convertToShipCoords(e.absPos, ship);

        if (entityInFrame(relPos, model.radius)) {
            
            Point3D points[model.numPoints];
            char codes[model.numPoints];
            for(int i = 0; i < model.numPoints; ++i) {
                points[i] = Transform(model.points[i], entity.orientation)+relPos;
                codes[i] = endPointCode(points[i]);
            }

            for(int i = 0; i < model.numLines; ++i) {
                int start = model.connections[i][0];
                int end   = model.connections[i][1];

                if (codes[start] & codes[end] == 0) {
                    Point2D startPoint = Point2D{points[start][0],points[start][1]}/points[start][2];
                    Point2D endPoint = Point2D{points[end][0],points[end][1]}/points[end][2];
                    int brightness = (int)(510/(points[start][2]+points[end][2]));
                    printLine(buff, startPoint, endPoint, brightness);
                }

                else if (codes[start] & codes[end] == 0) {
                    
                }
            }

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
