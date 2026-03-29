#ifndef PET_H
#define PET_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <memory>
#include <map>

enum class PetState {
    IDLE,
    WALK,
    SLEEP,
    DRAGGING
};

struct TextureDeleter {
    void operator()(SDL_Texture* t) const { if (t) SDL_DestroyTexture(t); }
};

/**
 * The Pet class manages the pet's state machine, animation, and movement logic.
 */
class Pet {
public:
    Pet(SDL_Window* window, SDL_Renderer* renderer);
    ~Pet() = default;

    void update(float dt);
    void render();
    void handleEvent(SDL_Event& e);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    // Mapping states to animation frame textures
    std::map<PetState, std::vector<std::unique_ptr<SDL_Texture, TextureDeleter>>> animations;
    
    // Position and movement
    float x, y; 
    float velocityX;
    
    // State management
    int displayIndex;
    PetState state;
    float stateTimer;
    
    // Animation timing
    int currentFrame;
    float frameTimer;

    // Interaction state
    bool isDragging;
    int dragOffsetX, dragOffsetY;

    // Local configuration cache
    bool cfgCanWalk, cfgCanSleep, cfgCanDrag, cfgCanClick;
    float cfgWalkSpeed, cfgGravity, cfgFrameDuration;
    int cfgWidth, cfgHeight;

    void changeState();
    void updateScreenBounds();
    void loadAnimations();
    void loadStateTextures(PetState s, const std::string& folder, int frameCount);
};

#endif // PET_H
