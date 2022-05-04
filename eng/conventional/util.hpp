#ifndef UTIL_HPP
#define UTIL_HPP

#include <array>
#include <cstdio>
#include <cmath>

class vec3 {
    private:
        std::array<float, 3> elems;
    public:
        uint8_t length = 3;

        vec3(float x, float y, float z);
        ~vec3();
        void log();
        

        // Basic operations
        vec3& operator=(const vec3& other);
        float operator[](const uint8_t idx);
        float operator[](const uint8_t idx) const;
        bool operator==(const vec3& other);
        vec3 norm();
        
        // Vector operations
        vec3 operator+(const vec3& other);
        vec3 operator-(const vec3& other);
        float dot(const vec3& other);
        vec3 cross(const vec3& other);

        // Scalar operations
        vec3 operator*(float scalar);
        vec3 operator/(float scalar); 

};

class vec4 {
    private:
        std::array<float, 4> elems;
    public:
        uint8_t length = 4;
        
        vec4(float x, float y, float z, float w);
        vec4();
        ~vec4();
        void log();

        // Basic operations
        vec4& operator=(const vec4& other);
        float operator[](const uint8_t idx);
        float operator[](const uint8_t idx) const;
        bool operator==(const vec4& other);
        vec4 norm();
        
        // Vector operations
        vec4 operator+(const vec4& other);
        vec4 operator-(const vec4& other);
        float dot(const vec4& other);

        // Scalar operations
        vec4 operator*(float scalar);
        vec4 operator/(float scalar); 
};

class mat3 {
    private:
        std::array<std::array<float, 3>, 3> elems;
    public:
        uint8_t rows = 3;
        uint8_t cols = 3;
        
        mat3(float init_values[3][3]);
        mat3();
        ~mat3();
        void log();

        // Basic operations
        mat3& operator=(const mat3& other);
        std::array<float,3>& operator[](uint8_t idx);
        const std::array<float,3>& operator[](uint8_t idx) const;
        bool operator==(const mat3& other);
        mat3 identity();

        // Vector operations
        vec3 operator*(const vec3& other);
        
        // Matrix operations
        mat3 operator*(const mat3& other);
};

class mat4 {
    private:
        std::array<std::array<float, 4>, 4> elems;
    public:
        uint8_t rows = 4;
        uint8_t cols = 4;

        mat4(float init_values[4][4]);
        mat4();
        ~mat4();
        void log();

        // Basic operations
        mat4& operator=(const mat4& other);
        std::array<float,4>& operator[](uint8_t idx);
        const std::array<float,4>& operator[](uint8_t idx) const; 
        bool operator==(const mat4& other);
        mat4 inverse();

        // Vector operations
        vec4 operator*(const vec4& other);
        
        // Matrix operations
        mat4 operator*(const mat4& other);
};

vec4 vec3_to_vec4(vec3);

mat4 translation_matrix(float, float, float);

mat4 x_rotation_matrix(float);
mat4 y_rotation_matrix(float);
mat4 z_rotation_matrix(float);


#endif
