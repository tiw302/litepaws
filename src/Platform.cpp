#include "../include/Platform.h"
#include <cstdlib>
#include <iostream>

#ifdef __linux__
#include <unistd.h>
#endif

void Platform::initialize() {
    const char* session = std::getenv("XDG_SESSION_TYPE");
    if (session) {
        std::string s(session);
        if (s == "wayland") {
            // Force X11 backend for global positioning (XWayland)
            // This is the standard way for desktop pets/widgets on Wayland
            setenv("SDL_VIDEODRIVER", "x11", 1); 
            std::cout << "[Platform] Running on Wayland, forcing XWayland for compatibility." << std::endl;
        } else {
            std::cout << "[Platform] Running on Native X11." << std::endl;
        }
    }
}

void Platform::applyWindowTweaks(SDL_Window* window) {
    if (!window) return;

    // Common tweaks for both X11/XWayland
    SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
    
    // Attempt to skip taskbar (WM specific, works on most X11 environments)
    // SDL handle this internally with SDL_WINDOW_SKIP_TASKBAR
}

std::string Platform::getSessionType() {
    const char* session = std::getenv("XDG_SESSION_TYPE");
    return session ? std::string(session) : "unknown";
}
