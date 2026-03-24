#ifndef PET_H
#define PET_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>

enum class PetState {
    IDLE,
    WALK_LEFT,
    WALK_RIGHT,
    SLEEP,
    DRAGGING
};

class Pet {
public:
    Pet(SDL_Window* window, SDL_Renderer* renderer, const std::string& texturePath);
    ~Pet();

    void update(float dt);
    void render();
    void handleEvent(SDL_Event& e);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    
    // Logic position (float for smooth movement)
    float x, y; 
    
    float velocityX, velocityY;
    
    int screenWidth, screenHeight;
    int displayIndex;
    
    PetState state;
    float stateTimer;
    
    // Animation
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
