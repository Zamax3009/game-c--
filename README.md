MyElementalRPG

MyElementalRPG is a modular, retro-minimalist 2D RPG engine built from scratch in C++ using Raylib and CMake. The project emphasizes clean code architecture, strong object-oriented design principles (encapsulation), and a flexible turn-based combat system.

Features

State Management: Fluid state machine supporting WorldMap, RegionHub, Inventory, and dynamic Combat states.

Turn-Based Combat System: Tactical fight loop featuring player skill sets, dynamic damage calculations, enemy counterattacks, and interactive victory/defeat screens.

Modular Ability System: Skill sets driven by an extensible Attack structure (damage, healing, and tactical effects).

Clean OOP Architecture: Full encapsulation of entities (Player, Enemy) with strict accessors, explicit constructors, and data safety.

Exploration & Minimap: Real-time position tracking, interactive world entities, and dynamic enemy radar overlay using raymath.h.

Building and Running

Prerequisites

C++17 compliant compiler (GCC, Clang, MSVC)

CMake (3.15+)

Raylib library installed or accessible

Build Instructions

# Generate build configuration
cmake -B build

# Build the project executable
cmake --build build

# Run the game (Linux/macOS)
./build/MyElementalRPG
