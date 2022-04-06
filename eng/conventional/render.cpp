#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <unistd.h>
#include <chrono>
#include "util.hpp"
#include <sstream>

#include "game.hpp"
#include "util.hpp"
#include "rawio.h"

#define MAX_X 255
#define MIN_X -255
#define MAX_Y 255
#define MIN_Y -255

#define FRAME_DELAY 1
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
const float FOV = PI/2;
const float ASP = 1.0f;
const float F = 1/tanf(FOV/2);

void render_frame(mat4& projection_matrix, int num_objects, Entity* game_objects) { 

    for(int object_idx = 0; object_idx < num_objects; object_idx++) {
        Entity& current_entity = game_objects[object_idx];
        vec4 points[current_entity.model.num_points];
       
        for(int point_idx = 0; point_idx < current_entity.model.num_points; point_idx++) { 
            vec4 current_point = vec3_to_vec4(current_entity.model.points[point_idx]);
            vec3 translation = current_entity.position;

            current_point = translation_matrix(translation[0], translation[1], translation[2]) * current_point;

            current_point = projection_matrix*current_point;
            current_point = current_point/current_point[3];
            current_point = current_point*255;

            points[point_idx] = current_point;
        }


        for(int connection_idx = 0; connection_idx < current_entity.model.num_connections; connection_idx++) {
            printf("reached\n"); 

            int index_1 = current_entity.model.connections[connection_idx][0];
            int index_2 = current_entity.model.connections[connection_idx][1];
            
            vec3 point_1 = current_entity.model.points[index_1];
            vec3 point_2 = current_entity.model.points[index_2];

            draw_absolute_vector(point_1[0], point_1[1], 0);
            draw_absolute_vector(point_2[0], point_2[1], 255); 
        }
    }

    draw_end_buffer();

}

int main(int argc, char** argv) {
    initialize_input_output();
    
    float projection_matrix_init_values[4][4] = { 
        {ASP*F,  0.f,  0.f,                                               0.f},
        {0.f,    F,    0.f,                                               0.f},
        {0.f,    0.f,  FAR_PLANE/(FAR_PLANE-CLOSE_PLANE), -(CLOSE_PLANE*FAR_PLANE)/(FAR_PLANE-CLOSE_PLANE)},
        {0.f,    0.f,  1.f,  0.f}
    };
    mat4 projection_matrix = mat4(projection_matrix_init_values);

    vec3 p1 = vec3(0, 0, 2);
    vec3 p2 = vec3(1, 0, 2);
    vec3 points[2] = {p1, p2};
    
    const int** connections = {{0, 1}};
    Model M = {2, points, 1, connections};
    Entity E = {M, mat4(), vec3(1, 1, 0)};
    Entity E_list[1] = {E};


    while(1) {
        
        render_frame(projection_matrix, 1, E_list);
        draw_buffer_switch();

    }

    return 0;
}
