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

Mat4 Multiply(const Mat4& A, const Mat4& B) {
    Mat4 C;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += A.m[k * 4 + row] * B.m[col * 4 + k];
            }
            C.m[col * 4 + row] = sum;
        }
    }
    return C;
}


Mat4 Translate(float x, float y, float z) {
    Mat4 mat;
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = z;
    return mat;
}

Mat4 Perspective(float fov, float aspect, float near, float far) {
    Mat4 mat;
    for(int i = 0; i < 16; i++) mat.m[i] = 0.0f; 

    float scale = 1.0f / tanf(fov / 2.0f);

    mat.m[0]  = scale / aspect;
    mat.m[5]  = scale;
    mat.m[10] = -(far + near) / (far - near);
    mat.m[11] = -1.0f;
    mat.m[14] = -(2.0f * far * near) / (far - near);
    
    return mat;
}
Mat4 Identity() {
    Mat4 matrix;
    for (int i = 0; i < 16; ++i) matrix.m[i] = 0.0f;
    matrix.m[0] = 1.0f;  
    matrix.m[5] = 1.0f;  
    matrix.m[10] = 1.0f;
    matrix.m[15] = 1.0f; 
    return matrix;
}

Mat4 RotateX(float angleRadians) {
    Mat4 matrix = Identity();
    float c = std::cos(angleRadians);
    float s = std::sin(angleRadians);

    matrix.m[5]  = c;  matrix.m[6]  = s;
    matrix.m[9]  = -s; matrix.m[10] = c;
    
    return matrix;
}

Mat4 RotateY(float angleRadians) {
    Mat4 matrix = Identity();
    float c = std::cos(angleRadians);
    float s = std::sin(angleRadians);

    matrix.m[0]  = c; matrix.m[2]  = -s;
    matrix.m[8]  = s; matrix.m[10] = c;
    
    return matrix;
}

#endif