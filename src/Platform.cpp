#include "../include/Platform.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>
#include <cstdlib>

void Platform::initialize() {
    // Force X11 for consistency on all Linux distros
    setenv("SDL_VIDEODRIVER", "x11", 1);
}

void Platform::applyWindowTweaks(SDL_Window* window) {
    if (!window) return;

    // Get X11 Info from SDL
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(window, &wmInfo)) {
        Display* display = wmInfo.info.x11.display;
        Window xwindow = wmInfo.info.x11.window;

        // Try to set ARGB Visual if supported
        XVisualInfo vinfo;
        if (XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo)) {
            XSetWindowAttributes attr;
            attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
            attr.border_pixel = 0;
            XChangeWindowAttributes(display, xwindow, CWColormap | CWBorderPixel, &attr);
        }
    }

    SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
}

std::string Platform::getSessionType() {
    return "x11";
}
