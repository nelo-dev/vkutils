@echo off

glslc.exe shader/simple.vert -o shader/simple_vert.spv
glslc.exe shader/simple.frag -o shader/simple_frag.spv
glslc.exe shader/post.vert -o shader/post_vert.spv
glslc.exe shader/post.frag -o shader/post_frag.spv
glslc.exe shader/staticpost.frag -o shader/staticpost_frag.spv

REM g++ -Ofast -c ./lib/vk_mem_alloc/vk_mem_alloc.cpp -o ./lib/vk_mem_alloc/vk_mem_alloc_win64.o
REM gcc -Ofast basic_cube.c -o vkuTestBasicCube ../lib/vk_mem_alloc/vk_mem_alloc_win64.o ../src/vkutils.c -lstdc++ -lglfw3 -lm -lvulkan-1
REM vkuTestBasicCube.exe

gcc -Og postprocessing_cube.c -o vkuTestPostProcessingCube ../lib/vk_mem_alloc/vk_mem_alloc_win64.o ../src/vkutils.c -lstdc++ -lglfw3 -lm -lvulkan-1
vkuTestPostProcessingCube.exe