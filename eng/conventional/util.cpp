#include "util.hpp"

// Constructors

vec3::vec3(float x, float y, float z) {
    elems = {x, y, z};
}

vec4::vec4(float x, float y, float z, float w) {
    elems = {x, y, z, w};
}

mat3::mat3(float input[3][3]) {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            elems[row][col] = input[row][col];
        }
    }
}
mat3::mat3() {
    elems = { {0.0f} }; // Initialize everything to 0.0f
}

mat4::mat4(float input[4][4]) {
    for (int row = 0; row < 4; row++) {
        for(int col = 0; col < 4; col++) {
            elems[row][col] = input[row][col];
        }
    }
}

mat4::mat4() {
    elems = { {0} };
}

// Destructors (Currently all properties are stack initialized, so there's nothing to destroy!)

vec3::~vec3(){}
vec4::~vec4(){}
mat3::~mat3(){}
mat4::~mat4(){}

// Log functions

void vec3::log() {
    printf("<%3f, %3f, %3f>\n", elems[0], elems[1], elems[2]);
}

void vec4::log() {
    printf("<%3f, %3f, %3f, %3f>\n", elems[0], elems[1], elems[2], elems[3]);
}

void mat3::log() {
    printf("%3f, %3f, %3f\n", elems[0][0], elems[0][1], elems[0][2]);
    printf("%3f, %3f, %3f\n", elems[1][0], elems[1][1], elems[1][2]);
    printf("%3f, %3f, %3f\n", elems[2][0], elems[2][1], elems[2][2]);
}

void mat4::log() {
    printf("%3f, %3f, %3f, %3f\n", elems[0][0], elems[0][1], elems[0][2], elems[0][3]);
    printf("%3f, %3f, %3f, %3f\n", elems[1][0], elems[1][1], elems[1][2], elems[1][3]);
    printf("%3f, %3f, %3f, %3f\n", elems[2][0], elems[2][1], elems[2][2], elems[2][3]);
    printf("%3f, %3f, %3f, %3f\n", elems[3][0], elems[3][1], elems[3][2], elems[3][3]);
}

// =

vec3& vec3::operator=(const vec3& other) {
    elems = {other[0], other[1], other[2]}; 
    return *this;
}

vec4& vec4::operator=(const vec4& other) {
    elems = {other[0], other[1], other[2], other[3]}; 
    return *this;
}

mat3& mat3::operator=(const mat3& other) {
    elems = other.elems;
    return *this;
}

mat4& mat4::operator=(const mat4& other) {
    elems = other.elems;
    return *this;
}

// []

float vec3::operator[](uint8_t idx) {return elems[idx];}
float vec4::operator[](uint8_t idx) {return elems[idx];}

float vec3::operator[](uint8_t idx) const {return elems[idx];}
float vec4::operator[](uint8_t idx) const {return elems[idx];}

std::array<float,3> mat3::operator[](uint8_t idx) {return elems[idx];}
std::array<float,4> mat4::operator[](uint8_t idx) {return elems[idx];}

// ==

bool vec3::operator==(const vec3& other) {return elems == other.elems;}
bool vec4::operator==(const vec4& other) {return elems == other.elems;}
bool mat3::operator==(const mat3& other) {return elems == other.elems;}
bool mat4::operator==(const mat4& other) {return elems == other.elems;}

// norm

vec3 vec3::norm() {
    float length = sqrt(elems[0]*elems[0] + elems[1]*elems[1] + elems[2]*elems[2]);
    vec3 other = *this/length;
    return other;
}

vec4 vec4::norm() {
    float length = sqrt(elems[0]*elems[0] + elems[1]*elems[1] + elems[2]*elems[2] + elems[3]*elems[3]);
    vec4 other = *this/length;
    return other;
}

// Vector +

vec3 vec3::operator+(const vec3& other) {
    float x = elems[0] + other[0];
    float y = elems[1] + other[1];
    float z = elems[2] + other[2];

    return vec3(x, y, z);
}

vec4 vec4::operator+(const vec4& other) {
    float x = elems[0] + other[0];
    float y = elems[1] + other[1];
    float z = elems[2] + other[2];
    float w = elems[3] + other[3];

    return vec4(x, y, z, w);
}

// Vector -

vec3 vec3::operator-(const vec3& other) {
    float x = elems[0] - other[0];
    float y = elems[1] - other[1];
    float z = elems[2] - other[2];

    return vec3(x, y, z);
}

vec4 vec4::operator-(const vec4& other) {
    float x = elems[0] - other[0];
    float y = elems[1] - other[1];
    float z = elems[2] - other[2];
    float w = elems[3] - other[3];

    return vec4(x, y, z, w);
}

// Dot product

float vec3::dot(const vec3& other) { return elems[0]*other[0] + elems[1]*other[1] + elems[2]*other[2]; }
float vec4::dot(const vec4& other) { return elems[0]*other[0] + elems[1]*other[1] + elems[2]*other[2] + elems[3]*other[3]; }

// Cross product

vec3 vec3::cross(const vec3& other) {
    float x = elems[1]*other[2] - other[1]*elems[2];
    float y = elems[2]*other[0] - other[2]*elems[0];
    float z = elems[0]*other[1] - elems[1]*other[0];

    return vec3(x, y, z);
}

// Scalar multiplication

vec3 vec3::operator*(float scalar) { return vec3(elems[0]*scalar, elems[1]*scalar, elems[2]*scalar); }
vec4 vec4::operator*(float scalar) { return vec4(elems[0]*scalar, elems[1]*scalar, elems[2]*scalar, elems[3]*scalar); }

// Scalar division

vec3 vec3::operator/(float scalar) { return vec3(elems[0]/scalar, elems[1]/scalar, elems[2]/scalar); }
vec4 vec4::operator/(float scalar) { return vec4(elems[0]/scalar, elems[1]/scalar, elems[2]/scalar, elems[3]/scalar); }

// Matrix-vector multiplication

vec3 mat3::operator*(const vec3& other) {
    float x = elems[0][0] * other[0] + elems[0][1] * other[1] + elems[0][2] * other[2];
    float y = elems[1][0] * other[0] + elems[1][1] * other[1] + elems[1][2] * other[2];
    float z = elems[2][0] * other[0] + elems[2][1] * other[1] + elems[2][2] * other[2];

    return vec3(x, y, z);
}

vec4 mat4::operator*(const vec4& other) { 
    float x = elems[0][0] * other[0] + elems[0][1] * other[1] + elems[0][2] * other[2] + elems[0][3] * other[3];
    float y = elems[1][0] * other[0] + elems[1][1] * other[1] + elems[1][2] * other[2] + elems[1][3] * other[3];
    float z = elems[2][0] * other[0] + elems[2][1] * other[1] + elems[2][2] * other[2] + elems[2][3] * other[3];
    float w = elems[3][0] * other[0] + elems[3][1] * other[1] + elems[3][2] * other[2] + elems[3][3] * other[3];
    
    return vec4(x, y, z, w);
}

// Matrix-Matrix multiplication (chonky)

mat3 mat3::operator*(const mat3& other) {
    mat3 result = mat3();

    for (int row = 0; row < 3; row++) {
        for(int col = 0; col < 3; col++) {
            result[row][col] = elems[row][0] * other[0][col] + elems[row][1] * other[1][col] + elems[row][2] * elems[2][col];
        }
    }

    return result;
}

mat4 mat4::operator*(const mat4& other) {
    mat4 result = mat4();
    
    for (int row = 0; row < 3; row++) {
        for(int col = 0; col < 3; col++) {
            result[row][col] = elems[row][0] * other[0][col] + elems[row][1] * other[1][col] + elems[row][2] * elems[2][col];
        }
    }

    return result;
}
