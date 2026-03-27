#include "../include/Platform.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>
#include <cstdlib>

void Platform::initialize() {
    setenv("SDL_VIDEODRIVER", "x11", 1);

    // Try to find a 32-bit ARGB visual and set the hint for SDL
    Display* display = XOpenDisplay(NULL);
    if (display) {
        XVisualInfo vinfo;
        // Request a 32-bit TrueColor visual
        if (XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo)) {
            char buf[32];
            // Format the visual ID as a hexadecimal string for the environment variable
            snprintf(buf, sizeof(buf), "0x%lx", vinfo.visualid);
            // Use SDL_SetHint for better compatibility than setenv for this specific hint
            SDL_SetHint(SDL_HINT_VIDEO_X11_VISUALID, buf);
            std::cout << "SDL Hint set: SDL_HINT_VIDEO_X11_VISUALID=" << buf << std::endl;
        } else {
            std::cerr << "Warning: Could not find a 32-bit TrueColor visual for transparency." << std::endl;
        }
        XCloseDisplay(display);
    } else {
        std::cerr << "Warning: Could not open X11 display to set visual ID hint." << std::endl;
    }
}

void Platform::applyWindowTweaks(SDL_Window* window) {
    if (!window) return;

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(window, &wmInfo)) {
        Display* display = wmInfo.info.x11.display;
        Window xwindow = wmInfo.info.x11.window;

        // 1. Force Sticky (Visible on all workspaces)
        Atom stateSticky = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
        Atom stateAbove = XInternAtom(display, "_NET_WM_STATE_ABOVE", False);
        Atom netWmState = XInternAtom(display, "_NET_WM_STATE", False);
        
        Atom states[] = { stateSticky, stateAbove };
        XChangeProperty(display, xwindow, netWmState, XA_ATOM, 32, PropModeReplace, (unsigned char*)states, 2);

        // 2. Force Skip Taskbar
        Atom skipTaskbar = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False);
        XChangeProperty(display, xwindow, netWmState, XA_ATOM, 32, PropModeAppend, (unsigned char*)&skipTaskbar, 1);
        
        XFlush(display);
    }
}

std::string Platform::getSessionType() {
    return "x11";
}
