#include "../include/Pet.h"
#include <iostream>
#include <cstdlib>

Pet::Pet(SDL_Window* window, SDL_Renderer* renderer, const std::string& texturePath)
    : window(window), renderer(renderer), state(PetState::IDLE), stateTimer(0), 
      frame(0), frameTimer(0), isDragging(false) 
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
        // Fallback: Create a simple colored surface
        surface = SDL_CreateRGBSurface(0, 64, 64, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 100, 100));
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    frameWidth = surface->w / 4; // Assuming 4 frames horizontal strip for simplicity
    frameHeight = surface->h;
    
    // If width < 4 (fallback), fix it
    if (frameWidth < 1) frameWidth = surface->w;
    
    frameCount = 4; // Hardcoded for now

    SDL_FreeSurface(surface);

    // Initial position
    int wx, wy;
    SDL_GetWindowPosition(window, &wx, &wy);
    x = static_cast<float>(wx);
    y = static_cast<float>(wy);

    updateScreenBounds();
    changeState();
}

Pet::~Pet() {
    if (texture) SDL_DestroyTexture(texture);
}

void Pet::updateScreenBounds() {
    displayIndex = SDL_GetWindowDisplayIndex(window);
    if (displayIndex < 0) displayIndex = 0;

    SDL_Rect bounds;
    SDL_GetDisplayBounds(displayIndex, &bounds);
    screenWidth = bounds.w;
    screenHeight = bounds.h;
    // Note: bounds.x and bounds.y are offsets. We need to handle global coords if we use them.
    // However, for clamping, we need to know the global bounds.
    // Let's store the bounds rect.
    // Re-using screenWidth/Height as strict local sizes might be confusing if not careful.
    // Let's just use the bounds in update.
}

void Pet::changeState() {
    // Simple random state machine
    int r = rand() % 100;
    if (r < 40) {
        state = PetState::IDLE;
        velocityX = 0;
        velocityY = 0;
        stateTimer = 2.0f + (rand() % 3);
    } else if (r < 70) {
        state = PetState::WALK_RIGHT;
        velocityX = 50.0f; // pixels per second
        velocityY = 0;
        stateTimer = 3.0f + (rand() % 4);
    } else if (r < 90) {
        state = PetState::WALK_LEFT;
        velocityX = -50.0f;
        velocityY = 0;
        stateTimer = 3.0f + (rand() % 4);
    } else {
        state = PetState::SLEEP;
        velocityX = 0;
        velocityY = 0;
        stateTimer = 5.0f + (rand() % 5);
    }
}

void Pet::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            int mx, my;
            SDL_GetGlobalMouseState(&mx, &my);
            int wx, wy;
            SDL_GetWindowPosition(window, &wx, &wy);
            isDragging = true;
            SDL_CaptureMouse(SDL_TRUE);
            state = PetState::DRAGGING;
            dragOffsetX = mx - wx;
            dragOffsetY = my - wy;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            isDragging = false;
            SDL_CaptureMouse(SDL_FALSE);
            changeState();
        }
    }
}

void Pet::update(float dt) {
    if (isDragging) {
        int mx, my;
        SDL_GetGlobalMouseState(&mx, &my);
        x = mx - dragOffsetX;
        y = my - dragOffsetY;
        SDL_SetWindowPosition(window, (int)x, (int)y);
        updateScreenBounds(); // Update bounds in case we dragged to another monitor
        return;
    }

    stateTimer -= dt;
    if (stateTimer <= 0) {
        changeState();
    }

    // Move
    x += velocityX * dt;
    y += velocityY * dt;

    // Boundary checks
    // We need global bounds of the current display
    SDL_Rect bounds;
    if (SDL_GetDisplayBounds(displayIndex, &bounds) != 0) {
        // Fallback if failed
        bounds = {0, 0, 1920, 1080}; 
    }

    // Check collisions with screen edges
    // Left
    if (x < bounds.x) {
        x = bounds.x;
        velocityX = -velocityX; // Bounce or stop? Let's turn around.
        if (state == PetState::WALK_LEFT) {
             state = PetState::WALK_RIGHT;
             velocityX = 50.0f;
        }
    }
    // Right
    if (x + frameWidth > bounds.x + bounds.w) {
        x = (bounds.x + bounds.w) - frameWidth;
        if (state == PetState::WALK_RIGHT) {
             state = PetState::WALK_LEFT;
             velocityX = -50.0f;
        }
    }
    // Bottom (Walk on bottom of screen)
    // "Taskbar" height is tricky to get without specific platform calls. 
    // We'll just stick to the bottom of the screen bounds minus some padding.
    int floorY = bounds.y + bounds.h - frameHeight;
    
    // Simple gravity/floor snapping logic
    if (y < floorY) {
        y += 100.0f * dt; // Gravity
    } else if (y > floorY) {
        y = floorY;
    }

    SDL_SetWindowPosition(window, (int)x, (int)y);

    // Animation
    frameTimer += dt;
    if (frameTimer > 0.2f) { // 5 FPS animation
        frameTimer = 0;
        frame = (frame + 1) % frameCount;
    }
}

void Pet::render() {
    // Clear with transparent color (if supported) or just clear
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); 
    SDL_RenderClear(renderer);

    // Render texture
    // Assuming horizontal strip: [Idle][WalkL][WalkR][Sleep] or just Frames?
    // Let's just use the 'frame' index for now simplistically.
    SDL_Rect srcRect = { frame * frameWidth, 0, frameWidth, frameHeight };
    SDL_Rect dstRect = { 0, 0, frameWidth, frameHeight };
    
    // Flip if walking left?
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (state == PetState::WALK_LEFT) flip = SDL_FLIP_HORIZONTAL;

    SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, 0, NULL, flip);

    SDL_RenderPresent(renderer);
}
