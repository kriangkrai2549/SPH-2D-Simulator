#include"renderer.h"
#include<vector>

Renderer::Renderer(){
    window = nullptr;
    renderer = nullptr;
    texture = nullptr;
}

Renderer::~Renderer(){
    shutDown();
}

bool Renderer::init(int width,int height){
    this->width = width;
    this->height = height;

    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        return false;
    }

    window = SDL_CreateWindow(
        "My window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,height,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    return window && renderer && texture;
}



void Renderer::clear(){
    SDL_RenderClear(renderer);
}

void Renderer::show(){
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}

void Renderer::shutDown(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void Renderer::update_texture(const std::vector<uint32_t> &pixel_buffer){
  SDL_UpdateTexture(
    texture,
    NULL,
    pixel_buffer.data(),
    this->width * sizeof(uint32_t)
  );

}

















