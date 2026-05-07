#ifndef SOLVER_H
#define SOLVER_H




#include<vector>
#include"particle.h"
#include"spatialhashgrid.h"
#include"SPH_const.h"
#include"thread_pool.h"

void thread_pool_density_pressure_solver(ThreadPool& thread_pool,std::vector<Particle>& particles,SpatialHashGrid& grid,float k,float rest_density,float smooth_radius,const SPHConst& sph_const,int number_thread);
void thread_pool_force_solver(ThreadPool &threads,std::vector<Particle>& particles,int number_thread,SpatialHashGrid& grid,float smooth_radius,float sigma,float mu,const SPHConst& sph_const);
void super_motion_solver(std::vector<Particle>& particles,float delta_time,float max_x,float max_y,float damping_factor);
void interact_froce_solver_pull(std::vector<Particle>& particles,SpatialHashGrid& grid,Vec2 interact_center_pos,float interact_radius,const SPHConst& sph_const);
#endif