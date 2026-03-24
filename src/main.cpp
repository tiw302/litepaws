#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <memory>
#include "../include/Config.h"
#include "../include/Pet.h"

// Custom deleters for RAII
struct SDLDeleter {
    void operator()(SDL_Window* w) const { if (w) SDL_DestroyWindow(w); }
    void operator()(SDL_Renderer* r) const { if (r) SDL_DestroyRenderer(r); }
};

int main(int argc, char* argv[]) {
    // Force X11 on Wayland systems if global positioning is required
    // SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Config::getInstance().load("config/settings.ini");
    const std::string texturePath = Config::getInstance().get("texture_path", "assets/pet.png");
    const int winW = Config::getInstance().getInt("width", 128);
    const int winH = Config::getInstance().getInt("height", 128);

    // Use smart pointers for automatic cleanup
    std::unique_ptr<SDL_Window, SDLDeleter> window(SDL_CreateWindow(
        "LitePaws",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        winW, winH,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR
    ));

    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::unique_ptr<SDL_Renderer, SDLDeleter> renderer(SDL_CreateRenderer(
        window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    ));

    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);

    // Pet handles its own texture cleanup via RAII inside its class
    Pet pet(window.get(), renderer.get(), texturePath);

    bool quit = false;
    SDL_Event e;
    Uint64 lastTime = SDL_GetPerformanceCounter();
    const double freq = static_cast<double>(SDL_GetPerformanceFrequency());

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) quit = true;
            pet.handleEvent(e);
        }

        Uint64 currentTime = SDL_GetPerformanceCounter();
        float dt = static_cast<float>((currentTime - lastTime) / freq);
        lastTime = currentTime;

        // Cap dt to prevent logic jumps during lag
        if (dt > 0.1f) dt = 0.1f;

        pet.update(dt);
        pet.render();

        // CPU conservation: Don't spin if there's no work
        SDL_Delay(1); 
    }

    IMG_Quit();
    SDL_Quit();
    return 0;
}
