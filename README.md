# LitePaws - Lightweight Desktop Pet for Linux

[![Build Status](https://github.com/tiw302/litepaws/actions/workflows/build.yml/badge.svg)](https://github.com/tiw302/litepaws/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

LitePaws is a high-performance, low-resource desktop pet application designed for modern Linux environments (X11 and Wayland). Written in C++17 and utilizing SDL2, it aims for a memory footprint under 10MB while providing smooth animations and multi-monitor support.

## Features

- **X11 & Wayland Support**: Native compatibility via SDL2.
- **Ultra Low RAM**: Optimized for minimal memory usage (5-10MB target).
- **Multi-Monitor Aware**: Detects screen boundaries and moves seamlessly across displays.
- **Customizable**: Easy PNG-based animations and configuration.
- **GUI Config Editor**: Simple Python-based tool to adjust settings without editing files manually.

## Directory Structure

- `src/`: C++ source files.
- `include/`: Header files.
- `assets/`: PNG assets and sprites.
- `config/`: Configuration files (`settings.ini`).
- `tools/`: Utility scripts (Config Editor).
- `bin/`: Compiled executables.

## Prerequisites

You will need the following packages (Ubuntu/Debian example):

```bash
sudo apt-get update
sudo apt-get install g++ make libsdl2-dev libsdl2-image-dev python3-tk
```

## Building

To compile the project, run:

```bash
make
```

## Running

Launch the pet:

```bash
./bin/litepaws
```

To edit settings (Image path, window size, speed):

```bash
python3 tools/config_editor.py
```

## Spritesheet Specification

By default, LitePaws expects a horizontal spritesheet with 4 frames. 
- Frame 0: Idle
- Frame 1-2: Walking
- Frame 3: Sleeping

The dimensions can be configured in the GUI tool or `config/settings.ini`.

## Transparency Note

Transparency depends on your Desktop Environment's compositor (e.g., Picom, GNOME, KDE). Ensure a compositor is running for the best experience. On some Wayland setups, transparency is handled automatically by the compositor.

## License

This project is open-source and ready for GitHub.
