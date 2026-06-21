cmake_minimum_required(VERSION 3.22.1)
project("djgaming")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_SOURCE_DIR}/ui
    ${CMAKE_SOURCE_DIR}/Unity
    ${CMAKE_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/includes/draw/Android_draw
    ${CMAKE_SOURCE_DIR}/includes/internal/ImGui
    ${CMAKE_SOURCE_DIR}/includes/internal/ImGui/backends
)

# Add Dobby static library
add_library(dobby STATIC IMPORTED)
set_target_properties(dobby PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/static/dobby/${ANDROID_ABI}/libdobby.a
)

# Your main library
add_library(${CMAKE_PROJECT_NAME} SHARED
    djgaming.cpp
    Socket.cpp
    MemoryReader.cpp
    game/game.cpp      # Add if you create this
    ui/menu.cpp        # Add if you create this
)

# Link libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    dobby
    android
    log
    z
    GLESv3
)

# Compiler flags
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -O2
    -fvisibility=hidden
    -fno-rtti
    -fno-exceptions
)