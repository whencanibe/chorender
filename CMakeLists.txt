cmake_minimum_required(VERSION 3.16)
project(opengl_base LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform-specific configurations
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Find required packages
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 3.3 REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)

# Find Assimp
find_package(assimp REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.h"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    glfw
    GLEW::GLEW
    assimp::assimp
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME}
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} GL X11 pthread Xrandr Xi dl)
endif()


# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    if(NOT MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    if(NOT MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE -O3)
    endif()
endif()
