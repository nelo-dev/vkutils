add_executable(example example.c)

target_include_directories(example PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(example PRIVATE vkutils)

# Resources and Shaders paths
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shader)
set(SHADER_BIN_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shader)

# Make sure shader output dir exists
file(MAKE_DIRECTORY ${SHADER_BIN_DIR})

# Gather all GLSL files
file(GLOB SHADER_FILES
    "${SHADER_SRC_DIR}/*.vert"
    "${SHADER_SRC_DIR}/*.frag"
)

# Build shader targets
foreach(SHADER IN LISTS SHADER_FILES)
    get_filename_component(FILE_NAME ${SHADER} NAME_WE)
    get_filename_component(EXT ${SHADER} EXT)
    string(REPLACE "." "_" STAGE ${EXT})
    set(SPIRV_FILE ${SHADER_BIN_DIR}/${FILE_NAME}${STAGE}.spv)

    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND glslc ${SHADER} -o ${SPIRV_FILE}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader: ${FILE_NAME}${STAGE}.spv"
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV_FILE})
endforeach()

# Create a custom target for shaders
add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_OUTPUTS})

# Copy resources to binary dir
add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RESOURCE_DIR}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
    COMMENT "Copying resources to output directory."
)

# Ensure shaders are compiled before running example
add_dependencies(example compile_shaders)
