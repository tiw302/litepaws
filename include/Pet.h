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

    // Animation sequences
    std::map<PetState, std::vector<std::unique_ptr<SDL_Texture, TextureDeleter>>> animations;
    
    float x, y; 
    float velocityX;
    
    int displayIndex;
    PetState state;
    float stateTimer;
    
    int currentFrame;
    float frameTimer;

    bool isDragging;
    int dragOffsetX, dragOffsetY;

    // Config cache
    bool cfgCanWalk, cfgCanSleep, cfgCanDrag, cfgCanClick;
    float cfgWalkSpeed, cfgGravity, cfgFrameDuration;
    int cfgWidth, cfgHeight;

    void changeState();
    void updateScreenBounds();
    void loadAnimations();
    void loadStateTextures(PetState s, const std::string& folder, int frameCount);
};

#endif // PET_H
