# Axis [![CMake](https://github.com/SimmyPeet/Axis/actions/workflows/CMake.yml/badge.svg)](https://github.com/SimmyPeet/Axis/actions/workflows/CMake.yml) [![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

Axis is a modern C++ game framework/library. The framework is designed to be highly modular and leverages the modern C++ features for safe and fast codes.

## Structure

Axis framework is sub-divided into many different modules for better project organization. Each of the modules is responsible for each specific task.

- **System:** This module provides our custom template container classes, math vectors, math matrices, and abstraction over low-level operating system function calls.
- **Window:** This module provides functionalities to display windows to the screen, process user events, and receive user inputs (mouse, keyboard, touch).
- **Graphics:** This module provides low-level abstractions over various next-gens Graphics APIs. Currently, the framework only supports **_Vulkan_**.
- **Renderer**: This module provides higher-level graphics functionalities such as: `SpriteBatch` class for fast and optimized 2D rendering
- **Core**: This module provides high-level application functionalities and classes to create a game.

## Plans

There are many features in the framework that aren't implemented but are coming in the future.

- **Window and System modules:** Currently, the framework's `Window` and `System` modules are only implemented for Windows operating system. There will be more implementations for more operating systems.
- **Audio module:** New `Audio` module will provide functionalities to load audio files, play audio and apply audio effects.
- **Full-blown engine:** The framework will further extend into a usable game engine.

## Getting Started

Currently, the framework doesn't provide any compiled binaries, Instead, build the framework and the dependencies from the source using CMake and link to your project.

**Clone this repository recursively:**

```zh
git clone --recursive https://github.com/SimmyPeet/Axis.git
```

**Graphics APIs' SDKs:**

The framework currently only uses **Vulkan** as a Graphics API backend. Make sure that **Vulkan® SDK** is installed in the system.

- _Vulkan SDK: [LunarG: Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)_

## Platforms

Currently, the framework only supports **Windows**, but many other operating system supports are coming in the future.

## Credits

External third-party libraries are used in this project.

- [glslang](https://github.com/KhronosGroup/glslang): Used in GLSL to SPIRV shader compilation.
- [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools): Used in GLSL to SPIRV shader compilation.
- [SPIRV-Headers](https://github.com/KhronosGroup/SPIRV-Headers): Used in GLSL to SPIRV shader compilation.
- [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Headers): Used in GLSL to SPIRV shader compilation.
- [freetype](https://github.com/freetype/freetype): Used in font-rendering.

## References

- [MonoGame](https://github.com/MonoGame/MonoGame): The project's structure is highly inspired by **MonoGame** framework.
- [DiligentCore](https://github.com/DiligentGraphics/DiligentCore): The graphics subsystem's structure is highly inspired by **DiligentGraphics/DiligentCore**.
