cmake_minimum_required(VERSION 3.10)
project(vkutils VERSION 1.0 LANGUAGES C CXX)

# Set build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable LTO and -Ofast
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE) # LTO
add_compile_options("$<$<CONFIG:Release>:-Ofast>")

# Find Vulkan and SDL3
find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

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

# Link vkutils against Vulkan, SDL3, and standard libraries
target_link_libraries(vkutils
    PUBLIC
        Vulkan::Vulkan
        SDL3::SDL3
    PRIVATE
        m
        stdc++
)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
