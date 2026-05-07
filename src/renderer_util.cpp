#include"renderer_util.h"
#include"renderer.h"
#include"color.h"
#include<vector>
#include<iostream>


template<typename T>
T clamp(T a,T min,T max){
  if(a < min)return min;
  if(a > max)return max;
  return a;
}



Draw::Draw(Renderer *renderer,int width,int height){
  this->renderer = renderer;
  this->width = width;
  this->height = height;
  pixel_buffer.resize(width*height);
}

void Draw::draw_point(uint32_t color,int x,int y){
    if(!check_boundary(x,y)){return;} 
    int pixel_index = (y * this->width) + x;
    uint32_t pre_color = pixel_buffer[pixel_index];

    uint8_t alpha = color >> 24 & 0xFF;
    uint8_t r = color >> 16 & 0xFF;
    uint8_t g = color >> 8 & 0xFF; 
    uint8_t b = color & 0xFF;

    uint8_t pre_r = pre_color >> 16 & 0xFF;
    uint8_t pre_g = pre_color >> 8 & 0xFF; 
    uint8_t pre_b = pre_color & 0xFF;

    float a = alpha/255.0f;
    float blended_r = clamp((a * r) + ((1-a) * pre_r), 0.0f, 255.0f);
    float blended_g = clamp((a * g) + ((1-a) * pre_g), 0.0f, 255.0f);
    float blended_b = clamp((a * b) + ((1-a) * pre_b), 0.0f, 255.0f);

    uint8_t new_r = (uint8_t)blended_r;
    uint8_t new_g = (uint8_t)blended_g;
    uint8_t new_b = (uint8_t)blended_b;


    uint32_t new_color = (alpha << 24)| (new_r << 16) | (new_g << 8) | new_b;

    pixel_buffer[pixel_index] = new_color;

}

void Draw::draw_square(uint32_t color,int s_x,int s_y,int height,int width){
    for(int y = 0 ;y < height;y++){
      for(int x = 0 ; x < width;x++){
        int cur_x = s_x + x;
        int cur_y = s_y + y;
        Draw::draw_point(color,cur_x,cur_y);
      }
    } 
  }

void Draw::draw_circle_not_fill(uint32_t color,int s_x,int s_y, int radius){
    for(int x = -radius; x <= radius;x++){
      int y = (int)sqrt(radius*radius - x*x);
      int cur_x = s_x + x;
      int cur_y = s_y + y;
      Draw::draw_point(color,cur_x,cur_y);
    }

    for(int x = -radius; x <= radius;x++){
      int y = -(int)sqrt(radius*radius - x*x);
      int cur_x = s_x + x;
      int cur_y = s_y + y;
      Draw::draw_point(color,cur_x,cur_y);
    }
  }


void Draw::draw_circle_fill(uint32_t color,int s_x,int s_y, int radius){
    float radius2 = radius*radius;
    for(int x = -radius; x <= radius;x++){
      for(int y = -radius; y <= radius;y++){
        float distance = x*x + y*y; 
        if( distance <= radius2){
          int cur_x = s_x + x;
          int cur_y = s_y + y;
          Draw::draw_point(color,cur_x,cur_y);
        }
      }
    }
  }



void Draw::draw_particle(uint32_t color,const std::vector<Particle>& particles,float smooth_radius){

    const float scale = 100.0f;
    std::vector<Vec2> pos;
    std::vector<int32_t> colors={RED,BLUE};
    for(int i = 0; i <particles.size();i++){
      const Particle& parti = particles[i];
      int x = (int)round(parti.pos.x * scale);
      int y = (int)round(parti.pos.y * scale);
      int radius = (int)round(smooth_radius * scale);
      Draw::draw_circle_fill(color,x,y,radius);
      pos.push_back({(float)x,(float)y});
    }

    // for(int i = 0; i < pos.size();i++){
    //   draw_circle_fill(WHITE,(int)pos[i].x,(int)pos[i].y,1);
    // }


}

bool Draw::check_boundary(int x,int y){
  if(x < 0 || x >= this->width) return false;
  if(y < 0 || y >= this->height)return false;
  return true;
}


void Draw::set_background(uint32_t color){
  std::fill(pixel_buffer.begin(),pixel_buffer.end(),color);
}


void Draw::draw_line(uint32_t color,Vec2 A, Vec2 B){
  if(A.x == B.x){
    if(A.y > B.y){std::swap(A,B);}
    for(int y = (int)A.y ; y <= (int)B.y;y++){
      Draw::draw_point(color,(int)A.x,y);
    }
  }else{
    if(A.x > B.x){std::swap(A,B);}
    float m = (float)(B.y - A.y) / (float)(B.x - A.x);
    float c = A.y - (A.x * m);
    for(int x = (int)A.x; x <=  (int)B.x;x++ ){
       int y = (int)(std::round(x*m + c));
       Draw::draw_point(color,x,y);
    }
  }
}


void Draw::draw_frame(uint32_t color,Vec2 A,Vec2 D){
  //A------B
  //|      |
  //|      |
  //C------D
  Vec2 B = {D.x,A.y};
  Vec2 C = {A.x,D.y};
  //A->B
  Draw::draw_line(color,A,B);
  //B->D
  Draw::draw_line(color,B,D);
  //D->C
  Draw::draw_line(color,D,C);
  //C->A
  Draw::draw_line(color,C,A);

}

void Draw::update_texture(){
    this->renderer->update_texture(this->pixel_buffer);
}










