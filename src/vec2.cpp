#include<iostream>
#include"vec2.h"



std::ostream& operator<<(std::ostream &os,const Vec2 vec){
   os << "(" << vec.x << "," << vec.y << ")";
   return os;
}


float dot(Vec2 a,Vec2 b){
   return (a.x*b.x) +(a.y*b.y); 
}


