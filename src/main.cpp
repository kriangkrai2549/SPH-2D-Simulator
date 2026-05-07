#define SDL_MAIN_HANDLED

#include<iostream>
#include<cmath>
#include<SDL2/SDL.h>




#include"renderer.h"
#include"renderer_util.h"
#include"color.h"
#include"spatialhashgrid.h"
#include"sph.h"
#include"solver.h"
#include"random.h"
#include"vec2.h"
#include"fps.h"
#include"timer.h"
#include"SPH_const.h"
#include"thread_pool.h"





void add_group_water(std::vector<Particle>& particles, Vec2 pos, int radius, float particle_radius) {
    float space = 0.1f;
    float step = (particle_radius * 2.0f) + space;
    int count = radius * 2;                          

    float start_x = pos.x - (count / 2.0f) * step;
    float start_y = pos.y - (count / 2.0f) * step;

    float circle_radius = radius * step;

    for (int ix = 0; ix < count; ix++) {
        float x = start_x + ix * step;
        float dx = x - pos.x;
        for (int iy = 0; iy < count; iy++) {
            float y = start_y + iy * step;
            float dy = y - pos.y;
            if (dx*dx + dy*dy <= circle_radius * circle_radius) {
                particles.push_back({{x, y}, {0.0f, 0.0f}, {0.0f, 0.0f}, 1.0f, 0.0f, 0.0f});
            }
        }
    }
}

int main(int argc, char* argv[]){

  // ── Window & rendering ────────────────────────────────────────────
  int width = 1200;          // window width  (pixels)
  int height = 800;         // window height (pixels)
  float scale = 100.0f;     // world units → pixels (1 unit = 100px)

  // ── World & grid ──────────────────────────────────────────────────
  float grid_size_x = 0.8f;               // size of each grid cell (world units)
  float grid_size_y = 0.8f;               // size of each grid cell (world units)

  int grid_x = width/(grid_size_x * scale); // number of grid cells horizontally
  int grid_y = height/(grid_size_y * scale);// number of grid cells vertically
  std::cout << grid_x << " " << grid_y << std::endl;
  float max_x = grid_size_x * grid_x;     // world width  boundary
  float max_y = grid_size_y * grid_y;     // world height boundary
  std::cout << max_x << " " << max_y << std::endl;

  // ── Particle ──────────────────────────────────────────────────────
  float radius_particle = 0.02f;  // visual radius of each particle
  float smooth_radius = 0.25f;    // SPH influence radius — how far a particle "feels" its neighbors

  // ── Fluid behavior ────────────────────────────────────────────────
  // Tweak these to change how the fluid behaves — have fun experimenting! 
  float rest_density = 75.0f;   // rest density      — how dense the fluid wants to be at rest
  float k = 0.01f;            // stiffness         — how hard particles resist compression
  float mu = 0.005f;            // viscosity         — 0.0 = water, high = honey 
  float sigma = 0.00025f;       // surface tension   — makes fluid blob together 
  float damping_factor = 0.5f;   // wall damping      — 1.0 = bouncy wall, 0.0 = dead stop


  // float delta_time = 0.5f;       // time step        — bigger = faster but more explosive 
  // float rest_density = 50.0f;   // rest density      — how dense the fluid wants to be at rest
  // float k = 0.00001f;            // stiffness         — how hard particles resist compression
  // float mu = 0.0012f;            // viscosity         — 0.0 = water, high = honey 
  // float sigma = 0.000001f;       // surface tension   — makes fluid blob together 
  // float damping_factor = 0.5f;   // wall damping      — 1.0 = bouncy wall, 0.0 = dead stop

  // ── Interaction ───────────────────────────────────────────────────
  float interact_radius = 1.0f;  // radius of mouse interaction (world units)

  // ── Thread ───────────────────────────────────────────────────
  int number_thread = 8;
  ThreadPool thread_pool(number_thread);
     


  bool running = true;
  std::cout << "running ..." << std::endl;
  Renderer renderer;
  Draw draw(&renderer,max_x * scale,max_y * scale);
  if(!renderer.init(max_x * scale,max_y * scale)){ 
    return 1;
  }
  SDL_Event event;

 
  // ── Create particle ─────────────────────────────────────────
  // ┌─────────────────────────────────────┐
  // │         Particle Grid Layout        │
  // │  Ex. ver=2, hor=3 looks like this:  │
  // │            + + +                    │
  // │            + + +                    │
  // └─────────────────────────────────────┘
  uint32_t ver = 44;
  uint32_t hor = 45;
  float space = 0.037f;// breathing room between particles (they like personal space)
  //Center the particle block in the world
  float padding_ver =   (max_y/2.0f) - (ver * (radius_particle + space));
  float padding_hor =   (max_x/2.0f) - (hor * (radius_particle + space));
  std::vector<Particle> particles;
  for(int x = 0;x < hor ;x++){ 
    float ix = padding_hor + ((radius_particle + space)*2.0f*x);
    for(int y = 0;y < ver ;y++){
       float iy = padding_ver + ((radius_particle + space)*2.0f*y);
       particles.push_back({{ix,iy},{0.0f,0.0f},{0.0f,0.0f}, 1.0f, 0.0f, 0.0f});
    }
  }


  // ── Create spatailHashGrid ─────────────────────────────────────────
  SpatialHashGrid grid(grid_x,grid_y,grid_size_x,grid_size_y,particles.size(),smooth_radius);
  if(grid.build(particles)!=0){return 1;}
  std::cout <<"Done build"<<std::endl;

  // ── Create fps ─────────────────────────────────────────
  Fps fps;
  std::vector<double> total_fps;

  // ── Create timer ─────────────────────────────────────────
  Timer timer(10);

  // ── Create Precompute Const ─────────────────────────────────────────
  SPHConst sph_const(smooth_radius);
  

  // ── Timing ─────────────────────────────────────────
  auto last_time = std::chrono::high_resolution_clock::now();
  float delta_time_fixed = 0.01f;
  float accumulator = 0.0f;

  int x_mouse,y_mouse;
  bool run = false;
  int count = 0;
  // ── Main loop ─────────────────────────────────────────
  while(running && count < 5000){
    uint32_t button = SDL_GetMouseState(&x_mouse,&y_mouse);
    fps.start();
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT){
        running = false;
      }
      if (event.type == SDL_KEYDOWN) {
          switch (event.key.keysym.sym) {
              case SDLK_SPACE:
                  run = !run; //Toggle space bar to run or stop
                  break;
              
         }
      }

    }
    
    if(button & SDL_BUTTON(SDL_BUTTON_LEFT)){
      //Want to interact pick one and uncomment it:
      
      // 1.Pull particle toward cursor
      // interact_froce_solver_pull(particles,grid,{x_mouse/scale,y_mouse/scale},interact_radius);
      // 2.Spawn a blob of water at cursor
      add_group_water(particles,{x_mouse/scale,y_mouse/scale},10,radius_particle);
    }



    if(run){

      auto current_time = std::chrono::high_resolution_clock::now();
      std::chrono::duration<float> elapsed = current_time - last_time;
      float frame_time = elapsed.count();
      last_time = current_time;

      if (frame_time > 0.1f) frame_time = 0.1f;

      accumulator += frame_time;

      while(accumulator >= delta_time_fixed){
        timer.start("building");
        grid.build(particles);
        timer.stop("building");
        // ── Density -> Pressure ─────────────────────────────────────────
        timer.start("thread_density_pressure_solver");
        thread_pool_density_pressure_solver(thread_pool,particles,grid,k,rest_density,smooth_radius,sph_const,number_thread);
        timer.stop("thread_density_pressure_solver");
  
        // ── Force ─────────────────────────────────────────
        timer.start("force_solver_thread");
        thread_pool_force_solver(thread_pool,particles,number_thread,grid,smooth_radius,sigma,mu,sph_const);
        timer.stop("force_solver_thread");
  
  
    
        // ── Velocity -> motion ─────────────────────────────────────────
        timer.start("super_motion_solver");
        super_motion_solver(particles,delta_time_fixed,max_x,max_y,damping_factor);
        timer.stop("super_motion_solver");

        accumulator -= delta_time_fixed;
      }
      count++;
    }else{

     last_time = std::chrono::high_resolution_clock::now();

    }

    //Draw every thing on canvas
    draw.set_background(WHITE_GRAY);
    draw.draw_particle(BLUE_TRAN,particles,radius_particle); 
    draw.draw_circle_not_fill(RED,x_mouse,y_mouse,interact_radius * scale); //Show interaction radius

    // copies your pixel_buffer → SDL texture (CPU side)
    draw.update_texture();

    // SDL sends that texture to GPU → displays on screen
    // ── Show Report ─────────────────────────────────────────      
    fps.end();
    double fps_avg = fps.get_fps();
    total_fps.push_back(fps_avg);
    std::cout <<"== Repor("<<count << ") =="<< std::endl;
    timer.show_report();
    std::cout << "FPS: " << fps_avg << std::endl;
    std::cout << " " << std::endl;
    renderer.show();
  }

      // ── Show Final Report ─────────────────────────────────────────      
  std::cout << "" << std::endl;
  std::cout <<"== Final Report(avg) =="<< std::endl;
  timer.show_final_report();
  std::cout << "avg_FPS: " << std::accumulate(total_fps.begin(), total_fps.end(), 0.0) / total_fps.size() << std::endl;;
  std::cout << "total_particle: " << particles.size() << std::endl;;

  // Cleanup and release all resources
  renderer.shutDown();
  return 0;
}


