# MyElementalRPG

MyElementalRPG is a high-performance 2D Action RPG (ARPG) engine built from scratch in C++ using Raylib. Originally designed as a turn-based RPG, it has evolved into a fluid, real-time combat experience featuring collision-based melee attacks, stamina-based movement, floating damage text, SQLite persistence, and an integrated HUD hotbar—all while preserving classic RPG progression (XP, gold, quests, inventory).

## Motivation

This project was created to explore custom 2D engine development in C++ without relying on heavy commercial game engines. The primary goal is to practice strong object-oriented design principles, clean data encapsulation, state-pattern architecture, and real-time performance optimization with a clean, minimalist retro aesthetic.

## Key Features

- **Real-Time ARPG Combat**: Smooth real-time collision-based melee attacks with cooldown timers.
- **Stamina & Movement System**: Fluid 8-directional movement paired with a stamina-cost dash mechanic and dynamic HUD feedback.
- **Floating Damage Text & Visual Juice**: Floating damage numbers with dynamic opacity decay and spatial movement.
- **Quick-Use Hotbar**: Dynamic exploration HUD allowing instant consumption of Health and Mana potions (`[1]` / `[2]`).
- **SQLite Game Persistence**: Transactional save and load system tracking player stats, inventory items, quest flags, map states, and enemy status.
- **World & Sub-Area Progression**: Open world exploration, building interiors, NPC dialogue parsing via JSON, fast-travel map matrix, and unlockable boss/arena sub-areas.

## Controls

### Exploration & ARPG Movement
- **W / A / S / D** or **Arrow Keys**: Move character
- **LEFT SHIFT** / **SPACE**: Dash (consumes Stamina)
- **E**: Collect world items / Pick up loot drops
- **F**: Interact with NPCs / Enter shops / Confirm fast travel selection
- **I**: Toggle Inventory screen
- **J**: Toggle Quest log
- **Q**: Toggle Fast Travel / World Map menu
- **F5**: Save game progress (SQLite database)
- **ESC**: Pause game / Back out of menus

### Real-Time Combat & Hotbar
- **K** / **Left Mouse Click**: Trigger melee attack hitbox
- **1**: Quick-use Health Elixir from Hotbar
- **2**: Quick-use Mana Potion from Hotbar
- **V**: Toggle Buy/Sell mode when inside the Shop interface

## How to Run

1. Download the release `.zip` archive from the project's Releases page.
2. Extract the contents of the `.zip` file to a folder.
3. Open the extracted folder.
4. Double-click `MyElementalRPG.exe` to launch the game.

*Note: Keep any included `.dll` files and `assets/` directory in the same folder as the executable.*

## Building from Source

### Prerequisites
- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake (v3.15+)
- Raylib library installed
- SQLite3 C/C++ development headers

### Build Steps
```bash
# Generate build configuration
cmake -B build

# Build the project executable
cmake --build build

# Run the executable
./build/MyElementalRPG
