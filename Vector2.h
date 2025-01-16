#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

class Vector2{
public:
Vector2(float X = 0, float Y = 0);
float Magnitude();
float x;
float y;
Vector2& operator+=(Vector2 B);
Vector2& operator-=(Vector2 B);
Vector2& operator*=(Vector2 B);
Vector2& operator/=(Vector2 B);
Vector2& operator*=(float B);
Vector2& operator/=(float B);
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

inline Vector2& Vector2::operator+=(Vector2 B){
    this->x += B.x;
    this->y += B.y;
    return *this;
}

inline Vector2& Vector2::operator-=(Vector2 B){
    this->x -= B.x;
    this->y -= B.y;
    return *this;
}

inline Vector2& Vector2::operator*=(Vector2 B){
    this->x *= B.x;
    this->y *= B.y;
    return *this;
}

inline Vector2& Vector2::operator/=(Vector2 B){
    this->x /= B.x;
    this->y /= B.y;
    return *this;
}


// Vector x Float operations
inline Vector2 operator+(Vector2 A, double B){
    return Vector2(A.x+B, A.y+B);
}

inline Vector2 operator-(Vector2 A, double B){
    return Vector2(A.x-B, A.y-B);
}
inline Vector2 operator*(Vector2 A, float B){
    return Vector2(A.x*B, A.y*B);
}

inline Vector2 operator/(Vector2 A, float B){
    return Vector2(A.x/B, A.y/B);
}

inline Vector2& Vector2::operator*=(float B){
    this->x *= B;
    this->y *= B;
    return *this;
}

inline Vector2& Vector2::operator/=(float B){
    this->x /= B;
    this->y /= B;
    return *this;
}


class iVector2{
public:
iVector2(int X = 0, int Y = 0);
float Magnitude();
int x;
int y;
iVector2& operator=(Vector2 B);
iVector2& operator+=(iVector2 B);
iVector2& operator-=(iVector2 B);
iVector2& operator*=(iVector2 B);
iVector2& operator/=(iVector2 B);
iVector2& operator*=(int B);
iVector2& operator/=(int B);
};


inline iVector2::iVector2(int X, int Y){
    x = X;
    y = Y;
}


inline iVector2& iVector2::operator=(Vector2 B){
    this->x = B.x;
    this->y = B.y;
    return *this;
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

inline iVector2& iVector2::operator+=(iVector2 B){
    this->x += B.x;
    this->y += B.y;
    return *this;
}

inline iVector2& iVector2::operator-=(iVector2 B){
    this->x -= B.x;
    this->y -= B.y;
    return *this;
}

inline iVector2& iVector2::operator*=(iVector2 B){
    this->x *= B.x;
    this->y *= B.y;
    return *this;
}

inline iVector2& iVector2::operator/=(iVector2 B){
    this->x /= B.x;
    this->y /= B.y;
    return *this;
}


// Vector x Int operations
inline iVector2 operator+(Vector2 A, int B){
    return iVector2(A.x+B, A.y+B);
}

inline iVector2 operator-(Vector2 A, int B){
    return iVector2(A.x-B, A.y-B);
}

inline iVector2 operator*(Vector2 A, int B){
    return iVector2(A.x*B, A.y*B);
}

inline iVector2 operator/(Vector2 A, int B){
    return iVector2(A.x/B, A.y/B);
}

inline iVector2& iVector2::operator*=(int B){
    this->x *= B;
    this->y *= B;
    return *this;
}

inline iVector2& iVector2::operator/=(int B){
    this->x /= B;
    this->y /= B;
    return *this;
}
