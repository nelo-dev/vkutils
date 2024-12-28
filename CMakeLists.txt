cmake_minimum_required(VERSION 3.14)
project(vkutils)

set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(COMPILE_WARNING_AS_ERROR ON)
set(LINKER_LANGUAGE C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -g")

option(BUILD_EXAMPLES OFF)
option(BUILD_SHARED OFF)
option(SYSTEM_LIBRARIES OFF)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_DEBUG})
    set(CMAKE_LINKER_FLAGS ${CMAKE_LINKER_FLAGS_DEBUG})
endif()

set(SOURCE_FILES 
    src/vkutils.c
    lib/vk_mem_alloc/vk_mem_alloc.cpp
)

set(HEADER_FILES
    lib/stb/stb_image.h
    lib/vk_mem_alloc/vk_mem_alloc.h
    src/vkutils.h
)

macro(build_library LINKING_TYPE)
    message("Building as ${LINKING_TYPE}")
    add_library(vkutils ${LINKING_TYPE} ${SOURCE_FILES} ${HEADER_FILES})
endmacro()

find_package(Vulkan REQUIRED)
find_package(cglm REQUIRED)
find_package(glfw3 REQUIRED)

if(SYSTEM_LIBRARIES)
    # Find and use system's `stb` and `VulkanMemoryAllocator` files.
endif()

if(BUILD_SHARED)
    build_library(SHARED)
else()
    build_library(STATIC)

    target_link_libraries(vkutils        
        Vulkan::Vulkan
        cglm::cglm
        glfw
        -lm
    )
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(run/)
endif()

include_directories(
    ${Vulkan_INCLUDE_DIR}
    ${cglm_INCLUDE_DIR}
    ${glfw3_INCLUDE_DIR}
)

target_include_directories(vkutils PUBLIC 
    src/ 
)

