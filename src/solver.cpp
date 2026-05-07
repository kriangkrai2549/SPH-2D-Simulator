#include<vector>
#include<thread>
#include"solver.h"
#include"spatialhashgrid.h"
#include"sph.h"
#include"particle.h"
#include"SPH_const.h"
#include"thread_pool.h"




// ── Density ───── Pressure ────────────────────────────────────
void density_pressure(std::vector<Particle>& particles,SpatialHashGrid& grid,float k,float rest_density,float smooth_radius,const SPHConst& sph_const,int start,int end){
   std::vector<int> neig_list;
   neig_list.reserve(64);
   for(int i = start;i < end ;i++){
      Particle& p = particles[i];
      grid.get_neighbor_index(p.pos,particles,neig_list,smooth_radius);
      p.density = compute_density(neig_list,particles,p,smooth_radius,sph_const);
      p.pressure = compute_pressure(p,k,rest_density);
   }
}

void thread_pool_density_pressure_solver(ThreadPool& thread_pool,std::vector<Particle>& particles,SpatialHashGrid& grid,float k,float rest_density,float smooth_radius,const SPHConst& sph_const,int number_thread){
   for(int i = 0 ; i < number_thread;i++){
      int start = particles.size()/number_thread * (i);
      int end = particles.size()/number_thread * (i+1); 
      if(i == number_thread -1){end = particles.size();}
      thread_pool.add_task([&,start,end]{
         density_pressure(particles,grid,k,rest_density,smooth_radius,sph_const,start,end);
      });
   }
   thread_pool.wait();
}

// ── Force ─────────────────────────────────────────
void force(std::vector<Particle>& particles,SpatialHashGrid& grid,float smooth_radius,float sigma,float mu,const SPHConst& sph_const,int start,int end){
   std::vector<int> neig_list;
   neig_list.reserve(64);
   for(int i = start ; i < end ;i++){
      Particle& p = particles[i];
      grid.get_neighbor_index(p.pos,particles,neig_list,smooth_radius);
      Vec2 tension_force = compute_surface_tension_force(neig_list,particles,p,smooth_radius,sigma,sph_const);
      Vec2 force = compute_prssure_viscosity_force(neig_list,particles,p,smooth_radius,mu,sph_const);
      Vec2 a_gravity{0.0f,0.5f};
      p.force = tension_force + force + a_gravity;
   }
}


void thread_pool_force_solver(ThreadPool &threads,std::vector<Particle>& particles,int number_thread,SpatialHashGrid& grid,float smooth_radius,float sigma,float mu,const SPHConst& sph_const){
   for(int i = 0 ; i < number_thread;i++){
      int start = particles.size()/number_thread * (i);
      int end = particles.size()/number_thread * (i+1); 
      if(i == number_thread -1){end = particles.size();}
      threads.add_task([&,start,end]{
          force(particles,grid,smooth_radius,sigma,mu,sph_const,start,end);
      });
   }
   threads.wait();
}



// ── Velocity ───── Position ────────────────────────────────────
void super_motion_solver(std::vector<Particle>& particles,float delta_time,float max_x,float max_y,float damping_factor){
  for(Particle& p:particles){
       Vec2 a = p.force / p.mass;
        p.velocity += a * delta_time; 
        p.pos += p.velocity * delta_time; 
        if(p.pos.x > max_x){
            p.pos.x = max_x;
            p.velocity.x *= -1.0f * damping_factor;
        }
        if(p.pos.y > max_y){
            p.pos.y = max_y;
            p.velocity.y *= -1.0f * damping_factor;
        }
        if(p.pos.x <= 1e-6f){
            p.pos.x = 0.0f;
            p.velocity.x *= -1.0f * damping_factor;
        }
        if(p.pos.y <= 1e-6f){
            p.pos.y = 0.0f;
            p.velocity.y *= -1.0f * damping_factor;
        }
  }
}




// ── User Interaction ────────────────────────────────────
void interact_froce_solver_pull(std::vector<Particle>& particles,SpatialHashGrid& grid,Vec2 interact_center_pos,float interact_radius,const SPHConst& sph_const){
  static std::vector<int> neig_list;
  grid.get_neighbor_index(interact_center_pos,particles,neig_list,interact_radius);
  for(int i:neig_list){
    Particle& p = particles[i];
    Vec2 force = compute_interact_force_pull(interact_center_pos,p.pos,interact_radius,sph_const);
    p.force += force;  
  }
} 

