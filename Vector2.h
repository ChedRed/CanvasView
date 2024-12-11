#pragma once
#include <cmath>

class Vector2{
float magnitude;
public:
Vector2(float X = 0, float Y = 0);
float Magnitude();
float x;
float y;
};

inline Vector2::Vector2(float X, float Y){
    x = X;
    y = Y;
}

inline float Vector2::Magnitude(){
    return std::sqrt((x*x)+(y*y));
}


// Vector x Vector operations
inline Vector2 operator+(Vector2 A, Vector2 B){
    return Vector2(A.x+B.x, A.y+B.y);
}

inline Vector2 operator-(Vector2 A, Vector2 B){
    return Vector2(A.x-B.x, A.y-B.y);
}

inline Vector2 operator*(Vector2 A, Vector2 B){
    return Vector2(A.x*B.x, A.y*B.y);
}

inline Vector2 operator/(Vector2 A, Vector2 B){
    return Vector2(A.x/B.x, A.y/B.y);
}

inline Vector2 operator+=(Vector2 A, Vector2 B){
    return Vector2(A.x+B.x, A.y+B.y);
}

inline Vector2 operator-=(Vector2 A, Vector2 B){
    return Vector2(A.x-B.x, A.y-B.y);
}

inline Vector2 operator*=(Vector2 A, Vector2 B){
    return Vector2(A.x*B.x, A.y*B.y);
}

inline Vector2 operator/=(Vector2 A, Vector2 B){
    return Vector2(A.x/B.x, A.y/B.y);
}


// Vector x Float operations
inline Vector2 operator*(Vector2 A, float B){
    return Vector2(A.x*B, A.y*B);
}

inline Vector2 operator/(Vector2 A, float B){
    return Vector2(A.x/B, A.y/B);
}

inline Vector2 operator*=(Vector2 A, float B){
    return Vector2(A.x*B, A.y*B);
}

inline Vector2 operator/=(Vector2 A, float B){
    return Vector2(A.x/B, A.y/B);
}

class iVector2{
public:
iVector2(int X = 0, int Y = 0);
float Magnitude();
int x;
int y;
};

inline iVector2::iVector2(int X, int Y){
    x = X;
    y = Y;
}

inline float iVector2::Magnitude(){
    return std::sqrt((x*x)+(y*y));
}


// Vector x Vector operations
inline iVector2 operator+(iVector2 A, iVector2 B){
    return iVector2(A.x+B.x, A.y+B.y);
}

inline iVector2 operator-(iVector2 A, iVector2 B){
    return iVector2(A.x-B.x, A.y-B.y);
}

inline iVector2 operator*(iVector2 A, iVector2 B){
    return iVector2(A.x*B.x, A.y*B.y);
}

inline iVector2 operator/(iVector2 A, iVector2 B){
    return iVector2(A.x/B.x, A.y/B.y);
}

inline iVector2 operator+=(iVector2 A, iVector2 B){
    return iVector2(A.x+B.x, A.y+B.y);
}

inline iVector2 operator-=(iVector2 A, iVector2 B){
    return iVector2(A.x-B.x, A.y-B.y);
}

inline iVector2 operator*=(iVector2 A, iVector2 B){
    return iVector2(A.x*B.x, A.y*B.y);
}

inline iVector2 operator/=(iVector2 A, iVector2 B){
    return iVector2(A.x/B.x, A.y/B.y);
}


// Vector x Float operations
inline iVector2 operator*(iVector2 A, int B){
    return iVector2(A.x*B, A.y*B);
}

inline iVector2 operator/(iVector2 A, int B){
    return iVector2(A.x/B, A.y/B);
}

inline iVector2 operator*=(iVector2 A, int B){
    return iVector2(A.x*B, A.y*B);
}

inline iVector2 operator/=(iVector2 A, int B){
    return iVector2(A.x/B, A.y/B);
}
