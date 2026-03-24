#include "../include/Pet.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Pet::Pet(SDL_Window* window, SDL_Renderer* renderer, const std::string& texturePath)
    : window(window), renderer(renderer), state(PetState::IDLE), stateTimer(0), 
      frame(0), frameTimer(0), isDragging(false) 
{
    srand(static_cast<unsigned int>(time(NULL)));

    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        std::cerr << "Warning: Texture not found, creating placeholder. Error: " << IMG_GetError() << std::endl;
        surface = SDL_CreateRGBSurface(0, 64, 64, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 100, 100));
    }

    texture.reset(SDL_CreateTextureFromSurface(renderer, surface));
    
    // Auto-calculate frame dimensions assuming a horizontal strip
    frameHeight = surface->h;
    frameWidth = frameHeight; // Default to square if single frame
    
    if (surface->w > surface->h) {
        frameCount = surface->w / surface->h;
        frameWidth = surface->w / frameCount;
    } else {
        frameCount = 1;
        frameWidth = surface->w;
    }

    SDL_FreeSurface(surface);

    int wx, wy;
    SDL_GetWindowPosition(window, &wx, &wy);
    x = static_cast<float>(wx);
    y = static_cast<float>(wy);

    updateScreenBounds();
    changeState();
}

void Pet::updateScreenBounds() {
    displayIndex = SDL_GetWindowDisplayIndex(window);
    if (displayIndex < 0) displayIndex = 0;
}

void Pet::changeState() {
    int r = rand() % 100;
    if (r < 40) {
        state = PetState::IDLE;
        velocityX = 0;
        stateTimer = 2.0f + (rand() % 3);
    } else if (r < 70) {
        state = PetState::WALK_RIGHT;
        velocityX = 60.0f;
        stateTimer = 3.0f + (rand() % 4);
    } else if (r < 90) {
        state = PetState::WALK_LEFT;
        velocityX = -60.0f;
        stateTimer = 3.0f + (rand() % 4);
    } else {
        state = PetState::SLEEP;
        velocityX = 0;
        stateTimer = 5.0f + (rand() % 5);
    }
}

void Pet::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx, my;
        SDL_GetGlobalMouseState(&mx, &my);
        int wx, wy;
        SDL_GetWindowPosition(window, &wx, &wy);
        
        isDragging = true;
        SDL_CaptureMouse(SDL_TRUE);
        state = PetState::DRAGGING;
        dragOffsetX = mx - wx;
        dragOffsetY = my - wy;
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        isDragging = false;
        SDL_CaptureMouse(SDL_FALSE);
        changeState();
    }
}

void Pet::update(float dt) {
    if (isDragging) {
        int mx, my;
        SDL_GetGlobalMouseState(&mx, &my);
        x = static_cast<float>(mx - dragOffsetX);
        y = static_cast<float>(my - dragOffsetY);
        SDL_SetWindowPosition(window, static_cast<int>(x), static_cast<int>(y));
        updateScreenBounds();
        return;
    }

    stateTimer -= dt;
    if (stateTimer <= 0) changeState();

    x += velocityX * dt;

    SDL_Rect bounds;
    if (SDL_GetDisplayBounds(displayIndex, &bounds) != 0) {
        bounds = {0, 0, 1920, 1080}; 
    }

    // Boundary check & Turn around
    if (x < bounds.x) {
        x = static_cast<float>(bounds.x);
        if (state == PetState::WALK_LEFT) {
            state = PetState::WALK_RIGHT;
            velocityX = 60.0f;
        }
    } else if (x + frameWidth > bounds.x + bounds.w) {
        x = static_cast<float>(bounds.x + bounds.w - frameWidth);
        if (state == PetState::WALK_RIGHT) {
            state = PetState::WALK_LEFT;
            velocityX = -60.0f;
        }
    }

    // Snap to floor (bottom of monitor)
    int floorY = bounds.y + bounds.h - frameHeight;
    y = static_cast<float>(floorY);

    SDL_SetWindowPosition(window, static_cast<int>(x), static_cast<int>(y));

    // Animation: Update frame based on state
    frameTimer += dt;
    if (frameTimer > 0.15f) {
        frameTimer = 0;
        if (frameCount > 1) {
            frame = (frame + 1) % frameCount;
        }
    }
}

void Pet::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); 
    SDL_RenderClear(renderer);

    SDL_Rect srcRect = { frame * frameWidth, 0, frameWidth, frameHeight };
    SDL_Rect dstRect = { 0, 0, frameWidth, frameHeight };
    
    SDL_RendererFlip flip = (velocityX < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    SDL_RenderCopyEx(renderer, texture.get(), &srcRect, &dstRect, 0, NULL, flip);
    SDL_RenderPresent(renderer);
}
