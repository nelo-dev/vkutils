cmake_minimum_required(VERSION 3.10)
project(vkutils VERSION 1.0 LANGUAGES C CXX)

# Set build type if not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set optimization flags based on build type
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW (assumes GLFW is installed or available)
find_package(glfw3 CONFIG REQUIRED)
if(NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW3 not found. Please install it.")
endif()

# Define the library
add_library(vkutils STATIC
    src/vkutils.c
    external/vk_mem_alloc/vk_mem_alloc.cpp
)

# Public include directories
target_include_directories(vkutils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_CURRENT_SOURCE_DIR}/external/vk_mem_alloc
    PRIVATE
        external/stb
)

# Link vkutils against Vulkan, GLFW, and standard libraries
target_link_libraries(vkutils
    PUBLIC
        Vulkan::Vulkan
        glfw
    PRIVATE
        m
        stdc++
)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add examples subdirectory
add_subdirectory(example)