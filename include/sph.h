#ifndef SPH_H
#define SPH_H


#include<vector>
#include"particle.h"
#include"spatialhashgrid.h"
#include"SPH_const.h"

float compute_density(
    const std::vector<int>& neighbor_list,
    const std::vector<Particle>& particles,
    const Particle& par,
    float smooth_radius,
    const SPHConst& sph_const
    
);

float compute_pressure(
    const Particle& par,
    float k,
    float rest_density
);


Vec2 compute_prssure_viscosity_force(const std::vector<int>& neighbor_list,
                                     const std::vector<Particle>& particles,
                                     const Particle& par,float smooth_radius,
                                     float mu,
                                     const SPHConst& sph_const
);


Vec2 compute_surface_tension_force(
    const std::vector<int>& neighbor_list,
    const std::vector<Particle>& particles,
    const Particle& par,
    float h,
    float sigma,
    const SPHConst& sph_const
);


Vec2 compute_interact_force_pull(
    Vec2 center,
    Vec2 particle_pos,
    float interact_radius,
    const SPHConst& sph_const
);



#endif