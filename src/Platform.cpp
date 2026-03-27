#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <SDL2/SDL_syswm.h>
#include "../include/Platform.h"
#include <iostream>
#include <cstdlib>

void Platform::initialize() {
    setenv("SDL_VIDEODRIVER", "x11", 1);

    Display* display = XOpenDisplay(NULL);
    if (display) {
        XVisualInfo vinfo;
        if (XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo)) {
            char buf[32];
            snprintf(buf, sizeof(buf), "0x%lx", vinfo.visualid);
            setenv("SDL_VIDEO_X11_VISUALID", buf, 1); // ✅ ใช้ setenv แทน SDL_SetHint
            std::cout << "Visual ID set: " << buf << std::endl;
        }
        XCloseDisplay(display);
    }
}

void Platform::applyWindowTweaks(SDL_Window* window) {
    if (!window) return;

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(window, &wmInfo)) {
        Display* display = wmInfo.info.x11.display;
        Window xwindow = wmInfo.info.x11.window;
       
        XSetWindowBackgroundPixmap(display, xwindow, None);
        XClearWindow(display, xwindow);
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
