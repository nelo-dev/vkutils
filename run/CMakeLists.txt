set(SHADER_FILES
	${CMAKE_SOURCE_DIR}/run/shader/post.frag
	${CMAKE_SOURCE_DIR}/run/shader/post.vert
	${CMAKE_SOURCE_DIR}/run/shader/simple.frag
	${CMAKE_SOURCE_DIR}/run/shader/simple.vert
	${CMAKE_SOURCE_DIR}/run/shader/staticpost.frag
)

macro(link target)
    target_link_libraries(${target}
        Vulkan::Vulkan
        cglm::cglm
        glfw
        vkutils
        -lm
    )
endmacro()

add_custom_target(shaders)

add_executable(vkuTestBasicCube basic_cube.c)
add_executable(vkuTestPostProcessingCube postprocessing_cube.c)

add_dependencies(vkuTestBasicCube shaders)
add_dependencies(vkuTestPostProcessingCube shaders)

link(vkuTestBasicCube)
link(vkuTestPostProcessingCube)

file(COPY ${CMAKE_SOURCE_DIR}/run/resources 
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/shader)

foreach(FILE ${SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)

  if(${FILE} MATCHES "(vert)")
	  set(OUTPUT_FILE "${FILE_WE}_vert.spv")
  elseif(${FILE} MATCHES "(frag)")
	  set(OUTPUT_FILE "${FILE_WE}_frag.spv")
  endif()

  message(${OUTPUT_FILE})
  
  add_custom_command(TARGET shaders
	COMMAND glslc ${FILE} -o "${CMAKE_CURRENT_BINARY_DIR}/shader/${OUTPUT_FILE}"
        MAIN_DEPENDENCY ${FILE}
	COMMENT "Compiling ${FILE} with glslc"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)
endforeach(FILE)

