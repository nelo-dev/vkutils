macro(link target)
    target_link_libraries(${target}
        Vulkan::Vulkan
        cglm::cglm
        glfw
        vkutils
        -lm
    )
endmacro()

add_executable(vkuTestBasicCube basic_cube.c)
add_executable(vkuTestPostProcessingCube postprocessing_cube.c)

link(vkuTestBasicCube)
link(vkuTestPostProcessingCube)

file(COPY ${CMAKE_SOURCE_DIR}/run/resources 
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
