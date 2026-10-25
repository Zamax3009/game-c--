# MyElementalRPG

MyElementalRPG is a lightweight 2D RPG engine built from scratch in C++ using Raylib. It features a modular state-based architecture, a tactical turn-based combat system, and clean object-oriented design with a retro-minimalist aesthetic.

## Motivation

This project was created to explore custom 2D engine development in C++ without relying on heavy commercial game engines. The primary goal is to practice strong object-oriented design principles, clean data encapsulation, and modular game architectures—all while maintaining a clean, minimalist retro aesthetic focused entirely on performance and gameplay mechanics.

## How to Run

1. Download the release `.zip` archive from the project's Releases page.
2. Extract the contents of the `.zip` file to a folder.
3. Open the extracted folder.
4. Double-click `MyElementalRPG.exe` to launch the game.

*Note: Keep any included `.dll` files in the same directory as the executable.*

## Controls

### Exploration & Navigation
* **W / A / S / D** or **Arrow Keys**: Move character
* **E**: Interact with world items
* **SPACE**: Engage enemy in combat (when nearby)
* **I**: Toggle Inventory screen
* **M**: Toggle Map view

### Combat System
* **1**: Quick Strike (Light Attack)
* **2**: Heavy Slash (Strong Attack)
* **3**: Inner Calm (Heal)
* **ENTER**: Confirm / Continue after Victory or Defeat
* **R** / **ESC**: Flee from battle

## Building from Source

### Prerequisites
* C++17 compatible compiler (GCC, Clang, or MSVC)
* CMake (v3.15+)
* Raylib library installed

### Build Steps
```bash
# Generate build configuration
cmake -B build

# Build the project executable
cmake --build build

# Run the executable
./build/MyElementalRPG
```
