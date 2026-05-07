#ifndef RENDOM_H
#define RENDOM_H

#include<random>

class Random{
  private:
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<float> dist;
  public:
    Random(float min,float max);
    float random_float();
};


#endif