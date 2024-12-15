#!/bin/bash

# Compile shaders
glslc shader/simple.vert -o shader/simple_vert.spv
glslc shader/simple.frag -o shader/simple_frag.spv
glslc shader/post.vert -o shader/post_vert.spv
glslc shader/post.frag -o shader/post_frag.spv
glslc shader/staticpost.frag -o shader/staticpost_frag.spv

# Compile Vulkan memory allocator
g++ -Ofast -c ../lib/vk_mem_alloc/vk_mem_alloc.cpp -o ../lib/vk_mem_alloc/vk_mem_alloc_linux64.o

# Compile basic cube program
gcc -Og -g basic_cube.c -o vkuTestBasicCube ../lib/vk_mem_alloc/vk_mem_alloc_linux64.o ../src/vkutils.c -lstdc++ -lglfw -lm -lvulkan

# Compile post-processing cube program
gcc -Og -g postprocessing_cube.c -o vkuTestPostProcessingCube ../lib/vk_mem_alloc/vk_mem_alloc_linux64.o ../src/vkutils.c -lstdc++ -lglfw -lm -lvulkan
