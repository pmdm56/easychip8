#ifndef SCREEN_H
#define SCREEN_H
#define SDL_MAIN_HANDLED 
#include "SDL.h"

class Screen
{
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
public:
    Screen(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Screen();
    void update(void const* buffer, int pitch);
};

#endif