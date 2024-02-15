#include "/opt/homebrew/Cellar/sdl2/2.30.0/include/SDL2/SDL.h"
//#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

class PrimordialGame { // Pure virtual class (all functions don't have defs)
    public:
    virtual const int getH() = 0;
    virtual const int getW() = 0;
    virtual SDL_Renderer *getRenderer() = 0;
};

class Sprite {
    SDL_Texture *tex;
    SDL_Rect dest;
    SDL_Rect src; // the drawing area I am trying to manipulate
    int xdir, ydir;
    bool good;
    PrimordialGame *g;
    public:
    Sprite(PrimordialGame *newG, string fname, int newX, int newY) {
        xdir = 1;
        ydir = 1;
        good = true;
        g = newG;
        SDL_Surface *bmp = SDL_LoadBMP("test.bmp");
        if (!bmp) { // bit map file
            cerr << "Error reading bit map" << endl;
            good = false;
            return;
        }
        tex = SDL_CreateTextureFromSurface(g->getRenderer(),bmp); // convert the image to the texture
        SDL_FreeSurface(bmp); // free up the memory of the surface in the cpu (pushed it to the graphics card in line above)
        src.x=0; src.y=0; src.w=20; src.h=20;
        dest=src;
        dest.x=newX; dest.y=newY;
    }
    void draw() {
        if (good) SDL_RenderCopy(g->getRenderer(),tex,&src,&dest);
    }
    void update() {
        dest.x += xdir;
        dest.y += ydir;
        if (dest.x > g->getW() || dest.x < 0) xdir *= -1;
        if (dest.y > g->getH() || dest.y < 0) ydir *= -1;
    }

    ~Sprite() {
        SDL_DestroyTexture(tex);
    }
};

class Game: public PrimordialGame { // concrete instance of PrimordialGame
    SDL_Renderer *ren;
    int w,h;
    bool running;
    vector<Sprite*> sprites;
    public:
    Game(string name="Enpty Game", int newW=640, int newH=480) {
        w = newW;
        h = newH;
        running = true;
        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            cout << "Failed to initialize the SDL2 library\n";
            running=false;
            return;
        }
        SDL_Window *window = SDL_CreateWindow(name.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,0);
        if(!window) {
            cout << "Failed to create window\n";
            running = false;
            return;
        }
        ren = SDL_CreateRenderer(window,-1,0); // my connection to my graphics card (needs to know what window to draw to)
    }
    void add(Sprite *s) { sprites.push_back(s); }
    const int getW() { return w; }
    const int getH() { return h; }
    SDL_Renderer *getRenderer() { // getter for ren
        return ren;
    }
    virtual void loop() {
        int count = 0;
        while (count < 10000) {
            SDL_PumpEvents();
            SDL_RenderClear(ren);
            for (auto s:sprites) s->draw();
            SDL_RenderPresent(ren);
            for (auto s:sprites) s->update();
            count++;
        }
    }

    ~Game() {
        SDL_DestroyRenderer(ren);
        SDL_Quit();
    }
};

Game *g;
int main() {
    Sprite *logo;
    g = new Game();
    for (int i=0; i<10; i++) {
        int x = rand() % g ->getW();
        int y = rand() % g->getH();
        logo = new Sprite(g,"test.bmp", 10,10);
        g->add(logo);
    }
    g->loop();
    delete logo;
    delete g;
} // calls the destructor