cmake_minimum_required(VERSION 3.16)

project(PreprocessOnly LANGUAGES CXX)

# Source to preprocess (change if needed)
set(SOURCE_FILE ${CMAKE_SOURCE_DIR}/../../main.cpp)
set(OUTPUT_FILE ${CMAKE_BINARY_DIR}/main.i)
message("Tools Source :" ${CMAKE_SOURCE_DIR})

# Run preprocessor with include paths
add_custom_command(
    OUTPUT ${OUTPUT_FILE}
    COMMAND ${CMAKE_CXX_COMPILER}
        /P /C /nologo
        /external:W0
        /external:I ${CMAKE_SOURCE_DIR}/../../vendor/glfw/include
        /external:I ${CMAKE_SOURCE_DIR}/../../vendor/glm
        /external:I ${CMAKE_SOURCE_DIR}/../../vendor/stb_image
        /external:I ${CMAKE_SOURCE_DIR}/../../vendor/tinyobjloader
        /external:I ${CMAKE_SOURCE_DIR}/../../vendor/json_nlohmann
        /external:I ${CMAKE_SOURCE_DIR}/../../vendor/spirv_reflect
        /external:I ${CMAKE_SOURCE_DIR}/../../vendor/tracy/public
        /external:I $ENV{VULKAN_SDK}/include
        /I ${CMAKE_SOURCE_DIR}/../../src
        /I ${CMAKE_SOURCE_DIR}/../../src/Core
        /Fi${OUTPUT_FILE}
        ${SOURCE_FILE}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Preprocessing ${SOURCE_FILE} to ${OUTPUT_FILE}"
)

add_custom_target(preprocess_main DEPENDS ${OUTPUT_FILE})
