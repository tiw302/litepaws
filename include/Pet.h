#ifndef PET_H
#define PET_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <memory>

enum class PetState {
    IDLE,
    WALK_LEFT,
    WALK_RIGHT,
    SLEEP,
    DRAGGING
};

// RAII Texture Deleter
struct TextureDeleter {
    void operator()(SDL_Texture* t) const { if (t) SDL_DestroyTexture(t); }
};

class Pet {
public:
    Pet(SDL_Window* window, SDL_Renderer* renderer, const std::string& texturePath);
    ~Pet() = default;

    // Disallow copying to prevent multiple deletions or invalid texture pointers
    Pet(const Pet&) = delete;
    Pet& operator=(const Pet&) = delete;

    void update(float dt);
    void render();
    void handleEvent(SDL_Event& e);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<SDL_Texture, TextureDeleter> texture;
    
    float x, y; 
    float velocityX, velocityY;
    
    int displayIndex;
    PetState state;
    float stateTimer;
    
    int frame;
    float frameTimer;
    int frameCount;
    int frameWidth;
    int frameHeight;

    bool isDragging;
    int dragOffsetX, dragOffsetY;

    void changeState();
    void updateScreenBounds();
};

#endif // PET_H
