#ifndef REDERER_UITL_H
#define REDERER_UITL_H

#include<SDL2/SDL.h>
#include"particle.h"
#include<vector>
// #include"renderer.h"
// #include"color.h"

struct Renderer;


class Draw{
  private:
    Renderer *renderer;
    std::vector<uint32_t>  pixel_buffer;
    int width;
    int height;
  public:
    Draw(Renderer *renderer,int width,int height);
    void draw_square(uint32_t color,int s_x,int s_y,int height,int width);
    void draw_circle_not_fill(uint32_t color,int s_x,int s_y, int radius);
    void draw_circle_fill(uint32_t color,int s_x,int s_y, int radius);
    void draw_point(uint32_t color,int x,int y);
    void draw_particle(uint32_t color,const std::vector<Particle>& particles,float smooth_radius);
    void draw_line(uint32_t color,Vec2 A, Vec2 B);
    void draw_frame(uint32_t color,Vec2 A,Vec2 D);

    bool check_boundary(int x,int y);
    // void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
    void set_background(uint32_t color);
    void update_texture();
};


#endif







