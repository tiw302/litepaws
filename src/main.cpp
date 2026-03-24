#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include "../include/Config.h"
#include "../include/Pet.h"

int main(int argc, char* argv[]) {
    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // 2. Load Config
    Config::getInstance().load("config/settings.ini");
    std::string texturePath = Config::getInstance().get("texture_path", "assets/pet.png");
    int winW = Config::getInstance().getInt("width", 128);
    int winH = Config::getInstance().getInt("height", 128);

    // 3. Create Window
    // Try to enable transparency
    // SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // Might help on some backends

    SDL_Window* window = SDL_CreateWindow(
        "LitePaws",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        winW, winH,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // 4. Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Set Blend Mode for transparency
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 5. Create Pet
    Pet pet(window, renderer, texturePath);

    // 6. Main Loop
    bool quit = false;
    SDL_Event e;
    
    Uint32 lastTime = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                   quit = true;
                }
            }
            pet.handleEvent(e);
        }

        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Cap dt to avoid spiraling if lag
        if (dt > 0.1f) dt = 0.1f;

        pet.update(dt);
        pet.render();

        // Low RAM usage optimization: Don't spin too fast if idle
        SDL_Delay(16); // ~60 FPS cap is handled by VSYNC usually, but this saves CPU
    }

    // 7. Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
