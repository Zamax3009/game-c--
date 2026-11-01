cmake_minimum_required(VERSION 3.16)
project(MyElementalRPG CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/raylib)

file(GLOB_RECURSE GAME_SOURCES "src/*.cpp")
file(GLOB IMGUI_SOURCES "external/imgui/*.cpp")
file(GLOB RLIMGUI_SOURCES "external/rlImGui/*.cpp")

add_executable(${PROJECT_NAME} 
    ${GAME_SOURCES} 
    ${IMGUI_SOURCES} 
    ${RLIMGUI_SOURCES}
    vendor/sqlite/sqlite3.c 
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    vendor/sqlite
    src 
    external/raylib/src 
    external/imgui 
    external/rlImGui
    external/json
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    raylib
)
