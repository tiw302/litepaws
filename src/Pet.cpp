#include "../include/Pet.h"
#include "../include/Config.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Pet::Pet(SDL_Window* window, SDL_Renderer* renderer)
    : window(window), renderer(renderer), state(PetState::IDLE), stateTimer(0), 
      currentFrame(0), frameTimer(0), frameDuration(0.15f), isDragging(false) 
{
    srand(static_cast<unsigned int>(time(NULL)));

    // Cache Config Settings
    Config& cfg = Config::getInstance();
    cfgCanWalk = cfg.getInt("can_walk", 1) == 1;
    cfgCanSleep = cfg.getInt("can_sleep", 1) == 1;
    cfgCanDrag = cfg.getInt("can_drag", 1) == 1;
    cfgCanClick = cfg.getInt("can_click", 1) == 1;
    cfgWalkSpeed = cfg.getFloat("walk_speed", 60.0f);
    cfgGravity = cfg.getFloat("gravity", 200.0f);

    loadAnimations();

    int wx, wy;
    SDL_GetWindowPosition(window, &wx, &wy);
    x = static_cast<float>(wx);
    y = static_cast<float>(wy);

    updateScreenBounds();
    changeState();
}

void Pet::loadStateTextures(PetState s, const std::string& folder, int frameCount) {
    if (frameCount <= 0) return;
    
    for (int i = 1; i <= frameCount; ++i) {
        std::string path = "assets/" + folder + "/" + std::to_string(i) + ".png";
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (surf) {
            animations[s].push_back(std::unique_ptr<SDL_Texture, TextureDeleter>(SDL_CreateTextureFromSurface(renderer, surf)));
            SDL_FreeSurface(surf);
        } else {
            std::cerr << "Could not load animation frame: " << path << std::endl;
        }
    }
}

void Pet::loadAnimations() {
    Config& cfg = Config::getInstance();
    loadStateTextures(PetState::IDLE, "idle", cfg.getInt("idle_frames", 1));
    loadStateTextures(PetState::WALK, "walk", cfg.getInt("walk_frames", 1));
    loadStateTextures(PetState::SLEEP, "sleep", cfg.getInt("sleep_frames", 1));

    // Fallback if no images found
    if (animations[PetState::IDLE].empty()) {
        SDL_Surface* surf = SDL_CreateRGBSurface(0, 64, 64, 32, 0, 0, 0, 0);
        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 100, 100));
        animations[PetState::IDLE].push_back(std::unique_ptr<SDL_Texture, TextureDeleter>(SDL_CreateTextureFromSurface(renderer, surf)));
        SDL_FreeSurface(surf);
    }
}

void Pet::updateScreenBounds() {
    displayIndex = SDL_GetWindowDisplayIndex(window);
    if (displayIndex < 0) displayIndex = 0;
}

void Pet::changeState() {
    if (isDragging) return;

    int r = rand() % 100;
    if (r < 40) {
        state = PetState::IDLE;
        velocityX = 0;
        stateTimer = 2.0f + (rand() % 4);
    } else if (r < 80 && cfgCanWalk) {
        state = PetState::WALK;
        velocityX = (rand() % 2 == 0) ? cfgWalkSpeed : -cfgWalkSpeed;
        stateTimer = 3.0f + (rand() % 5);
    } else if (cfgCanSleep) {
        state = PetState::SLEEP;
        velocityX = 0;
        stateTimer = 5.0f + (rand() % 10);
    } else {
        state = PetState::IDLE;
        velocityX = 0;
        stateTimer = 2.0f;
    }
    currentFrame = 0;
}

void Pet::handleEvent(SDL_Event& e) {
    if (!cfgCanDrag && !cfgCanClick) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (cfgCanDrag) {
            int mx, my, wx, wy;
            SDL_GetGlobalMouseState(&mx, &my);
            SDL_GetWindowPosition(window, &wx, &wy);
            isDragging = true;
            SDL_CaptureMouse(SDL_TRUE);
            state = PetState::DRAGGING;
            dragOffsetX = mx - wx;
            dragOffsetY = my - wy;
        }
        if (cfgCanClick) {
            // Placeholder for click interaction logic
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        if (isDragging) {
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
        x = static_cast<float>(mx - dragOffsetX);
        y = static_cast<float>(my - dragOffsetY);
    } else {
        stateTimer -= dt;
        if (stateTimer <= 0) changeState();

        x += velocityX * dt;

        SDL_Rect bounds;
        if (SDL_GetDisplayBounds(displayIndex, &bounds) != 0) {
            bounds = {0, 0, 1920, 1080}; 
        }

        // Boundary/Turning
        if (x < bounds.x) {
            x = static_cast<float>(bounds.x);
            velocityX = -velocityX;
        } else if (x + 128 > bounds.x + bounds.w) { // Assuming 128 as width, should be dynamic
            x = static_cast<float>(bounds.x + bounds.w - 128);
            velocityX = -velocityX;
        }

        // Snap to floor
        int floorY = bounds.y + bounds.h - 128; // Dynamic height
        if (y < floorY) y += cfgGravity * dt;
        if (y > floorY) y = static_cast<float>(floorY);
    }

    SDL_SetWindowPosition(window, static_cast<int>(x), static_cast<int>(y));

    // Animation frames
    auto& currentAnim = animations.count(state) ? animations[state] : animations[PetState::IDLE];
    if (!currentAnim.empty()) {
        frameTimer += dt;
        if (frameTimer > frameDuration) {
            frameTimer = 0;
            currentFrame = (currentFrame + 1) % currentAnim.size();
        }
    }
}

void Pet::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); 
    SDL_RenderClear(renderer);

    auto& currentAnim = animations.count(state) ? animations[state] : animations[PetState::IDLE];
    if (!currentAnim.empty()) {
        SDL_RendererFlip flip = (velocityX < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(renderer, currentAnim[currentFrame].get(), NULL, NULL, 0, NULL, flip);
    }

    SDL_RenderPresent(renderer);
}
