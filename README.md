# LitePaws (Work-in-Progress)

[![Build Status](https://github.com/tiw302/litepaws/actions/workflows/build.yml/badge.svg)](https://github.com/tiw302/litepaws/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

LitePaws is a high-performance, low-resource desktop pet application designed for modern Linux environments. Written in C++17 and utilizing SDL2, it aims for a memory footprint under 10MB while providing smooth animations and multi-monitor support.

## Features

- **X11 & Wayland Support**: Native compatibility via SDL2 (XWayland recommended).
- **Ultra Low RAM**: Optimized for minimal memory usage.
- **Multi-Monitor Aware**: Detects screen boundaries and moves seamlessly across displays.
- **Customizable**: Easy PNG-based animation sequences (idle, walk, sleep, drag).
- **GUI Config Editor**: Simple Python-based tool to adjust settings.

## Current Limitations & Transparency Issues
LitePaws is a work-in-progress personal project. I am continuously learning and improving the codebase. Currently, there are some known limitations:

- **Transparency on X11/i3wm**: Achieving perfect transparency with some window managers can be tricky. If you experience a black background instead of transparency, it is likely due to compositor settings or X11 visual configuration. Please ensure you have a compositor like `picom` running. I am actively looking for ways to improve this.
- **Wayland Support**: Currently, the application relies on XWayland for global window positioning. Native Wayland support is a goal for the future.

If you are an experienced Linux user and know how to improve these aspects, contributions are more than welcome!

## Directory Structure

- `src/`: C++ source files.
- `include/`: Header files.
- `assets/`: PNG assets and sprites (place your frames here!).
- `config/`: Configuration files (`settings.ini`).
- `tools/`: Utility scripts (Config Editor, Test Asset Generator).
- `bin/`: Compiled executables.

## Prerequisites

You will need the following packages (Ubuntu/Debian example):

```bash
sudo apt-get update
sudo apt-get install g++ make libsdl2-dev libsdl2-image-dev python3-tk python3-pillow
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

To edit settings:

```bash
python3 tools/config_editor.py
```

## License

This project is licensed under the [MIT License](LICENSE) - see the [LICENSE](LICENSE) file for details.
