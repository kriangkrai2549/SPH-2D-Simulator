#ifndef VEC2_H
#define VEC2_H
#include<iostream>
#include<cmath>



struct Vec2 { float x,y;
              Vec2():x(0),y(0){}
              Vec2(float x,float y):x(x),y(y){}
              static float distance(const Vec2& a,const Vec2& b){
                return (a-b).length();
              }
              static float distance2(const Vec2& a,const Vec2& b){
                Vec2 c = a-b; 
                return c.x*c.x + c.y*c.y;
              }

              Vec2 operator+(const Vec2& other) const{
                  return Vec2(x+other.x,y+other.y);
              }
              Vec2 operator-(const Vec2& other) const{
                  return Vec2(x-other.x,y-other.y);
              }
              Vec2& operator+=(const Vec2& other) {
                x += other.x;
                y += other.y;
               return *this;
              }
              Vec2& operator-=(const Vec2& other) {
                x -= other.x;
                y -= other.y;
               return *this;
              }
              Vec2 operator*(const Vec2& other) const{
                  return Vec2(x*other.x,y*other.y);
              }

              Vec2 operator*(const float& other) const{
                  return Vec2(x*other,y*other);
              }
              Vec2 operator/(const float& other) const{
                  return Vec2(x/other,y/other);
              }
              Vec2& operator*=(const float& other) {
                x *= other;
                y *= other;
                return *this;
              }
              Vec2& operator/=(const float& other) {
                x /= other;
                y /= other;
                return *this;
              }
              float length() const{
                return sqrt(x*x + y*y);
              }
              float length2() const{
                return x*x + y*y;
              }
            };

std::ostream& operator<<(std::ostream &os,const Vec2 vec);
float dot(Vec2 a,Vec2 b);
inline Vec2 operator*(float s,const Vec2& v){
  return v * s;
}

#endif
