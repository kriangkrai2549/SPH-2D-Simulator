#include"sph.h"
#include"vec2.h"
#include"SPH_const.h"



// ── Helper Performance Function ────────────────────────────────────────
float fast_inv_sqrt(float x){
    float xhalf = 0.5f * x;
    int i = *(int*)&x;           // reinterpret float bits as int
    i = 0x5f3759df - (i >> 1);   // magic number!
    x = *(float*)&i;
    x = x * (1.5f - xhalf * x * x); // one Newton iteration
    return x;
}

// ── Poly9 Kernel Function ─────────────────────────────────────────
float poly_kernel(float smooth_radius, float distance2,const SPHConst& sph_const) {
    if (distance2 >= sph_const.h2) return 0.0f;
    float term = sph_const.h2 - distance2;
    float constant = sph_const.poly; // 4 / PI
    return constant * term * term * term; 

}

float poly_kernel_diff(float smooth_radius,float distance,const SPHConst& sph_const){
  float r2 = distance*distance;
  if(r2 >= sph_const.h2){return 0.0f;}
  float term = (sph_const.h2-r2)*(sph_const.h2-r2);
  return  sph_const.poly_diff * distance * term;

}

Vec2 poly_kernel_gradient(float smooth_radius,Vec2 vec,const SPHConst& sph_const){
  float len2 = vec.length2();
  if(len2 <= 1e-14f ){return {0.0f,0.0f};}
  float inv_len = fast_inv_sqrt(len2);
  float len = len2 * inv_len;
  Vec2 direct = vec * inv_len;
  return  direct * poly_kernel_diff(smooth_radius,len,sph_const);
}

float poly_kernel_laplacian(float h, float r,const SPHConst& sph_const) {
    float r2 = r * r;
    if (r2 >= sph_const.h2) return 0.0f;
    float term = sph_const.h2 - r2;
    return sph_const.poly_lap * term * (4.0f * r2 - term);
}


// ── Spiky Kernel Function ─────────────────────────────────────────
float spiky_kernel_diff(float h, float r,const SPHConst& sph_const) {
  if (r >= sph_const.h || r <= 1e-7f) return 0.0f;
  float term = h - r;
  return sph_const.spiky_diff * term * term;
}

Vec2 spiky_kernel_gradient(float smooth_radius,Vec2 vec,const SPHConst& sph_const){
  float len2 = vec.length2();
  if(len2 <= 1e-14f ){return {0.0f,0.0f};}
  float inv_len = fast_inv_sqrt(len2);
  float len = len2 * inv_len;
  Vec2 direct = vec*inv_len;
  return  direct * spiky_kernel_diff(smooth_radius,len,sph_const);
}


// ── Color Field Kernel Function ─────────────────────────────────────────
float color_field(const std::vector<int>& neighbor_list,const std::vector<Particle>& particles,const Particle& par, float h,const SPHConst& sph_const) {
    float c = 0.0f;
    for (int i : neighbor_list) {
        const Particle& p = particles[i];
        Vec2 v = p.pos - par.pos;
        c += p.mass / p.density * poly_kernel(h, v.length2(),sph_const);
    }
    return c;
}

Vec2 color_laplacian_gradient(const std::vector<int>& neighbor_list,const std::vector<Particle>& particles,const Particle& par,float h,const SPHConst& sph_const){
    Vec2 n = {0.0f, 0.0f};
    float laplacian = 0.0f;
    for (int i : neighbor_list) {
        const Particle& p = particles[i];
        Vec2 v = p.pos - par.pos;
        float mass_over_den = p.mass / p.density;
        n += mass_over_den * poly_kernel_gradient(h, v,sph_const);

        float w_lap = poly_kernel_laplacian(h, v.length(),sph_const);
        laplacian += mass_over_den * w_lap;

    }
    float len2 = n.length2();
    float threshold = 0.0001f;
    if (len2 < threshold)return{0.0f,0.0f};
    float n_len = fast_inv_sqrt(len2);
    return n * n_len * laplacian;
}

// ── viscosity Kernel Function ─────────────────────────────────────────
float viscosity_kernel_laplacian(float smooth_radius,Vec2 vec,const SPHConst& sph_const){
  float r2 = vec.length2();
  float inv_r = fast_inv_sqrt(r2);
  float r = r2 * inv_r;
  if(r >= smooth_radius || r <= 1e-7f){return 0.0f;}
  return  sph_const.vis_lap * (smooth_radius - r);
}






float compute_density(const std::vector<int>& neighbor_list,const std::vector<Particle>& particles,const Particle& par,float smooth_radius,const SPHConst& sph_const){
  float density = 0.0f;
  for(int index : neighbor_list){
     const Particle& neig_par = particles[index];
     Vec2 v = neig_par.pos - par.pos; 
     density += neig_par.mass * poly_kernel(smooth_radius,v.length2(),sph_const); 
  }
  density += par.mass * poly_kernel(smooth_radius,0.0f,sph_const); 
  return density;
}

float compute_pressure(const Particle& par,float k,float rest_density){
  float pressure = 0.0f;
  pressure += k*(par.density - rest_density);
  return pressure;
}




Vec2 compute_prssure_viscosity_force(const std::vector<int>& neighbor_list,const std::vector<Particle>& particles,const Particle& par,float smooth_radius,float mu,const SPHConst& sph_const){
  Vec2 force = {0.0f,0.0f};;
  for(int index : neighbor_list){
     const Particle& neig_par = particles[index];
     Vec2 v = neig_par.pos - par.pos;
     float term_1  = neig_par.mass * (( (par.pressure + neig_par.pressure) / (2 *(neig_par.density)) ));
     Vec2 term_2 = spiky_kernel_gradient(smooth_radius,v,sph_const);

     Vec2 rela_velo = neig_par.velocity - par.velocity;
     Vec2 term = rela_velo / (neig_par.density);
     float la = viscosity_kernel_laplacian(smooth_radius,v,sph_const);

     force += term_1 * term_2;
     force += mu * neig_par.mass  * la * term;
  }
  return force;

}




Vec2 compute_surface_tension_force(const std::vector<int>& neighbor_list,const std::vector<Particle>& particles,const Particle& par, float h, float sigma,const SPHConst& sph_const) {
    return -sigma * color_laplacian_gradient(neighbor_list, particles, par, h,sph_const);
}






Vec2 compute_interact_force_pull(Vec2 center,Vec2 particle_pos,float interact_radius,const SPHConst& sph_const){
  Vec2 force = {0.0f,0.0f};
  Vec2 vec = center - particle_pos;
  float len2 = vec.length2();
  float inv_length = fast_inv_sqrt(len2);
  Vec2 direction = vec*inv_length;
  force += 0.9f * direction * poly_kernel(interact_radius,len2,sph_const);
  return force;
}

