#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <string>

/**
 * Platform-specific utilities and window manager tweaks.
 * Currently focused on X11 desktop environments.
 */
class Platform {
public:
    // Initialize platform-specific environment variables
    static void initialize();

    // Apply platform-specific window tweaks (Always on Top, etc.)
    static void applyWindowTweaks(SDL_Window* window);
    
    // Check if we are running under Wayland or X11
    static std::string getSessionType();
};

#endif // PLATFORM_H
