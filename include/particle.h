#ifndef PARTICLE_H
#define PARTICLE_H
#include"vec2.h"


struct Particle {Vec2 pos,
                      force,
                      velocity;
                 float mass,
                       density,
                       pressure;
};
std::ostream& operator<<(std::ostream &os, const Particle &p);


#endif 