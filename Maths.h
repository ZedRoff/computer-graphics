#ifndef MATHS_H
#define MATHS_H

#include <cmath>

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float _x, float _y) : x(_x), y(_y) {}
};

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Mat4 {
    float m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    
};

Mat4 Identity();
Mat4 RotateX(float angleRadians);
Mat4 RotateY(float angleRadians);
Mat4 Translate(float x, float y, float z);
Mat4 Perspective(float fov, float aspect, float near, float far);
Mat4 Multiply(const Mat4& A, const Mat4& B);

#endif