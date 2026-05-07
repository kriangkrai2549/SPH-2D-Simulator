#include<random>
#include"random.h"



Random::Random(float min,float max): dist{min,max}{};

float Random::random_float(){
  return dist(gen);
};
