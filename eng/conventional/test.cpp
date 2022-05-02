#include <iostream>
#include "util.hpp"

#define PI 3.141592

using namespace std;

int main(int argc, char** argv) {
    mat4 a = x_rotation_matrix(PI/2);
    mat4 b = y_rotation_matrix(PI/2);

    a.log();
    printf("\n");
    b.log();

    printf("\n");
    
    mat4 c = a * b;
    c.log();

    return 0;
}
