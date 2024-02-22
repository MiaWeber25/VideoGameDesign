#include "/opt/homebrew/Cellar/sdl2/2.30.0/include/SDL2/SDL.h"
//#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <map>

using namespace std;

class MediaManager {
    map<string,SDL_Texture*> textures;
    SDL_Renderer *ren;

    public:
    MediaManager(SDL_Renderer *newRen) {
        ren = newRen;
    }
    SDL_Texture * get(string fname) {
        if (textures.find(fname) == textures.end()) {
            SDL_Texture *tex;
            SDL_Surface *bmp = SDL_LoadBMP(fname.c_str()); // "test.bmp" etc.
            if (!bmp) { // bit map file
                cerr << "Error reading bit map" << endl;
                return NULL;
            }
            tex = SDL_CreateTextureFromSurface(ren,bmp); // convert the image to the texture (give it to the graphics card)
            SDL_FreeSurface(bmp); // free up the memory of the surface in the cpu (pushed it to the graphics card in line above)
            textures[fname]=tex; //set .bmp to texture in map class (assume texture doesn't exist)
        }
        return textures[fname];
    }
};
MediaManager *mm = NULL;

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
    bool good;
    PrimordialGame *g;

    protected:
    float x,y;
    float vx, vy;
    float ax, ay;

    public:
    Sprite(PrimordialGame *newG, string fname,
      float newX, float newY /*pixles*/, float newVx=1.0,
      float newVy=1.0/*pixles per sec*/, 
      float newAx=0.0, /* pixles per sec per sec*/
      float newAy=1.0) {
        g = newG;
        vx = newVx; // velocity This is like dt (can't integrate to get close, you can't control dt)
        vy = newVy; // velocity 
        good = true;
        if (mm == NULL) good=false;
        tex = mm->get(fname); // make use of MediaManager class to reference existing texture
        if (tex == NULL) good=false;
        src.x=0; src.y=0; src.w=20; src.h=20;
        dest=src;
        dest.x=newX; dest.y=newY; 
        x = newX; y = newY; 
        ax = newAx; ay = newAy; 
    }
    void draw() {
        if (good) SDL_RenderCopy(g->getRenderer(),tex,&src,&dest);
    }
    void update(float dt) {
        updatePosition(dt);
        dest.x=x; // convert the floats (used for integration) to ints so it can be placed on a pixel on the screen
        dest.y=y;
    }
    virtual void updatePosition(float dt) { // double integration in 2D:
        vx += ax*dt; // gravity
        vy += ay*dt;
        x += vx*dt;  // velocity
        y += vy*dt;
        if (x > g->getW() || x < 0) vx *= -1;
        if (y > g->getH() || y < 0) vy *= -1;
    }

    ~Sprite() {
        SDL_DestroyTexture(tex);
    }
};

enum Move{UP, DOWN, RIGHT, LEFT};

class Moveable {
    public:
    virtual void doMove(Move m)=0; // Pure virtual class (interface class = JAVA)
};

class Player: public Sprite, public Moveable {
    public:
    Player(PrimordialGame *g): Sprite(g,"bot.bmp",0,0) {

    }
    void doMove(Move m) {
        if (m == UP) y-=10;
        else if (m == DOWN) y+=10;
        else if (m == LEFT) x-=10;
        else if (m == RIGHT) x+=10;
    }

    void updatePosition(float dt) {
        Sprite::updatePosition(dt); // call the superclass function
    }
};

class Game: public PrimordialGame { // concrete instance of PrimordialGame
    SDL_Renderer *ren;
    int w,h;
    bool running;
    vector<Sprite*> sprites;
    vector<Moveable*> players; // also has a vector of movable things
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
        mm = new MediaManager(ren); // hand the MediaManager the needed renderer so it can create and store the texture
    }

    void add(Sprite *s) { sprites.push_back(s); }
    void add(Player *p) { players.push_back(p); }

    const int getW() { return w; }
    const int getH() { return h; }
    SDL_Renderer *getRenderer() { // getter for ren
        return ren;
    }

    void informPlayers(Move m) {
        for (auto p: players) p->doMove(m);
    }

    virtual void loop() {
        int count = 0;
        int lastTime = SDL_GetTicks();
        int firstTime = lastTime;
        float dt;
        SDL_Event e;
        bool done = false;
        while (!done) { 
            if (SDL_PollEvent(&e)) { // find out which event happened
                if (e.type == SDL_WINDOWEVENT) {
                    // here I could handle all window events
                    if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
                        done = true; // the user wants to quit the game (window was closed)
                    }
                }
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_a) informPlayers(LEFT);
                    if (e.key.keysym.sym == SDLK_w) informPlayers(UP);
                    if (e.key.keysym.sym == SDLK_s) informPlayers(DOWN);
                    if (e.key.keysym.sym == SDLK_d) informPlayers(RIGHT);
                    if(e.key.keysym.sym == SDLK_ESCAPE) done = true;
                }
                if (e.type == SDL_MOUSEMOTION) {
                    if (abs(e.motion.xrel) > abs(e.motion.yrel)) {
                        if (e.motion.xrel < 0) informPlayers(LEFT);
                        else informPlayers(RIGHT);
                    } else {
                        if (e.motion.yrel < 0) informPlayers (UP);
                        else informPlayers(DOWN); 
                    }
                }
            }
            SDL_PumpEvents();
            SDL_RenderClear(ren);
            for (auto s:sprites) s->draw();
            SDL_RenderPresent(ren);
            int newTime = SDL_GetTicks();
            dt = (float)(newTime - lastTime) / 1000.0; // how much time has passed in this function (in sec).
            lastTime = newTime; // reset the time
            for (auto s:sprites) s->update(dt);
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
    for (int i=0; i<100; i++) { // Max: 66997
        //float x = rand() % g ->getW();
        //float y = rand() % g->getH();
        float x = g->getW()/2;
        float y = g->getH()/2;
        float dx = rand() % 40 - 20; // randomize the velocities
        float dy = rand() % 40 - 20;
        logo = new Sprite(g,"test.bmp", x,y, dx, dy, 0.0, 10.0);
        g->add(logo);
    }
    Player *p = new Player(g);
    g->add((Sprite*)p); // add a new player to the game (pay attention to order -> in front)
    g->add((Player*)p);
    g->loop();
    delete logo;
    delete g;
} // calls the destructor