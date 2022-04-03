#ifndef UTIL_HPP
#define UTIL_HPP

#include <array>

template <typename T> class vec3 {
    private:
        std::array<T, 3> elems;
    public:
        uint8_t length = 3;

        vec3(T x, T y, T z);
        ~vec3();
        void log();
        

        // Basic operations
        vec3<T>& operator=(const vec3<T> other);
        void operator[](const uint8_t idx);
        bool operator==(const vec3<T> other);
        vec3<T>& norm();
        
        // Vector operations
        vec3<T>& operator+(const vec3<T> other);
        vec3& operator-(const vec3<T> other);
        T dot(const vec3<T> other);
        vec3& cross(const vec3<T> other);

        // Scalar operations
        vec3& operator*(T scalar);
        vec3& operator/(T scalar); 

};

template <typename T> class vec4 {
    private:
        std::array<T, 4> elems;
    private:
        uint8_t length = 4;
        
        vec4(T x, T y, T z, T w);
        ~vec4();
        void log();

        // Basic operations
        vec4<T>& operator=(const vec4<T> other);
        void operator[](const uint8_t idx);
        bool operator==(const vec4<T> other);
        vec4<T>& norm();
        
        // Vector operations
        vec4<T>& operator+(const vec4<T> other);
        vec4<T>& operator-(const vec4<T> other);
        T dot(const vec4<T> other);
        vec4<T>& cross(const vec4<T> other);

        // Scalar operations
        vec4<T>& operator*(T scalar);
        vec4<T>& operator/(T scalar); 
};

template <typename T> class mat3 {
    private:
        std::array<std::array<T, 3>, 3> elems;
    public:
        uint8_t rows = 3;
        uint8_t cols = 3;
        
        mat3(T init_values[3][3]);
        void log();

        // Basic operations
        mat3& operator=(const mat3<T> other);
        void operator[](const uint8_t idx);
        bool operator[](const mat3<T> other);

        // Vector operations
        vec3<T>& operator*(const vec3<T> other);
        
        // Matrix operations
        mat3<T>& operator*(const mat3<T> other);
};

template <typename T> class mat4 {
    private:
        std::array<std::array<T, 4>, 4> elems;
    public:
        uint8_t rows = 4;
        uint8_t cols = 4;

        mat4(T init_values[4][4]);
        void log();

        // Basic operations
        mat4<T>& operator=(const mat4<T> other);
        void operator[](const uint8_t idx);
        bool operator[](const mat3<T> other);

        // Vector operations
        vec4<T>& operator*(const vec4<T> other);
        
        // Matrix operations
        mat4<T>& operator*(const mat4<T> other);
};







#endif
