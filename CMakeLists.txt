cmake_minimum_required(VERSION 3.16)
project(MyElementalRPG CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Configura Raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/raylib)

# 2. Raccogli i file sorgente
file(GLOB_RECURSE GAME_SOURCES "src/*.cpp")
file(GLOB IMGUI_SOURCES "external/imgui/*.cpp")
file(GLOB RLIMGUI_SOURCES "external/rlImGui/*.cpp")

# 3. Crea l'eseguibile
add_executable(${PROJECT_NAME} 
    ${GAME_SOURCES} 
    ${IMGUI_SOURCES} 
    ${RLIMGUI_SOURCES}
)

# 4. Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    src 
    external/raylib/src 
    external/imgui 
    external/rlImGui
)

# 5. Link librerie
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)