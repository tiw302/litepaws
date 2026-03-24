#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>
#include "../include/Config.h"
#include "../include/Pet.h"
#include "../include/Platform.h"

struct SDLDeleter {
    void operator()(SDL_Window* w) const { if (w) SDL_DestroyWindow(w); }
    void operator()(SDL_Renderer* r) const { if (r) SDL_DestroyRenderer(r); }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    // FORCE X11 to use 32-bit visual for transparency
    // This is the "Magic" for Arch/i3wm + Picom
    setenv("SDL_VIDEO_X11_VISUALID", "", 0); 
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); // OpenGL handles transparency better on Linux

    // Force ARGB attributes before SDL_Init
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Platform::initialize();

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
    const int winW = Config::getInstance().getInt("width", 128);
    const int winH = Config::getInstance().getInt("height", 128);

    // SDL_WINDOW_OPENGL is MANDATORY for ARGB visual on many X11 setups
    std::unique_ptr<SDL_Window, SDLDeleter> window(SDL_CreateWindow(
        "LitePaws",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        winW, winH,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_OPENGL
    ));

    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    Platform::applyWindowTweaks(window.get());

    // Use Accelerated Renderer (OpenGL)
    std::unique_ptr<SDL_Renderer, SDLDeleter> renderer(SDL_CreateRenderer(
        window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    ));

    if (!renderer) {
        std::cerr << "Could not create hardware renderer, transparency might fail." << std::endl;
        renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE));
    }
    
    SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);

    // DEBUG: Final check if we got ARGB
    Uint32 fmt = SDL_GetWindowPixelFormat(window.get());
    std::cout << "[Final Check] Window Pixel Format: " << SDL_GetPixelFormatName(fmt) << std::endl;
    if (!SDL_ISPIXELFORMAT_ALPHA(fmt)) {
        std::cout << "!!! WARNING: Window still does not support ALPHA. Background will be BLACK. !!!" << std::endl;
    }

    Pet pet(window.get(), renderer.get());

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

        if (dt > 0.1f) dt = 0.1f;

        pet.update(dt);
        
        // Clear with FULL TRANSPARENCY
        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0); 
        SDL_RenderClear(renderer.get());
        
        pet.render();

        SDL_Delay(1); 
    }

    IMG_Quit();
    SDL_Quit();
    return 0;
}
