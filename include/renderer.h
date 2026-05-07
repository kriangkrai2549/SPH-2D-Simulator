#ifndef RENDER_H
#define RENDER_H

#include<SDL2/SDL.h>
#include<vector>




class Renderer{
  private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int width,height;
  public:
    Renderer();
    ~ Renderer();
    //Create renderer,window
    bool init(int width,int height);
    // Terminate every thing and return resource 
    void shutDown();
    //Clear window
    void clear();
    //Show
    void show();
    //Update texture
    void update_texture(const std::vector<uint32_t> &pixel_buffer);
    
};



#endif 