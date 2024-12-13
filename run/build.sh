#!/bin/bash

# Compile shaders
glslc shader/simple.vert -o shader/simple_vert.spv
glslc shader/simple.frag -o shader/simple_frag.spv
glslc shader/post.vert -o shader/post_vert.spv
glslc shader/post.frag -o shader/post_frag.spv

# Compile the program
gcc -Og -g main.c -o vkuTest ../lib/vk_mem_alloc/vk_mem_alloc_linux.o ../src/vkutils.c -lstdc++ -lglfw -lm -lvulkan

# Check if the compile was successful
if [ $? -eq 0 ]; then
    # Run the program if compilation succeeds
    ./vkuTest
else
    echo "Compilation failed."
    exit 1
fi