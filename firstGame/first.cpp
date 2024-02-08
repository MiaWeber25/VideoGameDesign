#include <SDL.h>
#include <iostream>

using namespace std;

int main() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Failed to initialize the SDL2 library\n";
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        680, 480,
        0);

    if(!window) {
        cout << "Failed to create window\n";
        return -1;
    }

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if(!window_surface) {
        cout << "Failed to get the surface from the window\n";
        return -1;
    }

    SDL_UpdateWindowSurface(window);
    SDL_PumpEvents();
    SDL_Delay(5000);
}