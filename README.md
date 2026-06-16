# 🌊 Fluid Simulation — SPH Physics (C++ & SFML 3.0)

Real-time **Smoothed Particle Hydrodynamics (SPH)** fluid simulation built in C++ using SFML 3.0.

![Demo](images/demo.png)

## ✨ Features
- Real fluid physics — pressure, viscosity, gravity
- Velocity-based colors (🔵 slow → 🔴 fast)
- Left click & drag — add particles
- Right click — clear all
- Smooth 60 FPS simulation

## 🛠️ Build Instructions

### Requirements
- MinGW (i686 / x86)
- vcpkg + SFML 3.0 (`x86-mingw-dynamic`)
- CMake 3.16+

### Steps
```bash
git clone https://github.com/snehal-thombare08/fluid-simulation-cpp.git
cd fluid-simulation-cpp
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake \
         -DSFML_DIR=C:/vcpkg/installed/x86-mingw-dynamic/share/sfml \
         -G "MinGW Makefiles"
mingw32-make
```

## 📥 Download & Run (Windows)
👉 [Download FluidSim-v1.0-Windows.zip](https://github.com/snehal-thombare08/fluid-simulation-cpp/releases/tag/v1.0)

Extract → run `FluidSim.exe` directly!

## 🎮 Controls
| Action | Result |
|--------|--------|
| Left Click + Drag | Add particles |
| Right Click | Clear all |

## 📄 License
MIT
