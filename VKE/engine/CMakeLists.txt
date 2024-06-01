cmake_minimum_required(VERSION 3.12)
project(VKE)

# Set your sources
file(GLOB_RECURSE SOURCES
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "*.h"
    "*.cpp"
)

# Add executable target
add_executable(Engine ${SOURCES} "utils.h" "Buffer.h" "Buffer.cpp" "FrameInfo.h" "FrameInfo.cpp" "Descriptors.h" "Descriptors.cpp" "Image.h" "Image.cpp" "Material.h" "Material.cpp" "Keyboard.h" "Keyboard.cpp")

# Set C++ standards and properties
set_target_properties(Engine PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    FOLDER "Engine"
)

# Include directories
target_include_directories(Engine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
)

# Find Vulkan package
find_package(Vulkan REQUIRED)

# Include GLFW, GLM, tinyobjloader, and stb_image
include(FetchContent)

# Fetch and include GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.4"
    GIT_SHALLOW TRUE
    UPDATE_DISCONNECTED TRUE
)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE)

FetchContent_MakeAvailable(glfw)

# Fetch and include GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm.git"
    GIT_TAG "1.0.1"
    GIT_SHALLOW TRUE
    UPDATE_DISCONNECTED TRUE
)

FetchContent_MakeAvailable(glm)

# Fetch and include tinyobjloader
FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY "https://github.com/tinyobjloader/tinyobjloader.git"
    GIT_TAG "v2.0.0rc13"
    GIT_SHALLOW TRUE
    UPDATE_DISCONNECTED TRUE
)

FetchContent_GetProperties(tinyobjloader)
if(NOT tinyobjloader_POPULATED)
  FetchContent_Populate(tinyobjloader)
  add_subdirectory(${tinyobjloader_SOURCE_DIR} ${tinyobjloader_BINARY_DIR})
endif()

# Fetch and include stb_image
FetchContent_Declare(
    stb
    GIT_REPOSITORY "https://github.com/nothings/stb.git"
    GIT_TAG "master"
    GIT_SHALLOW TRUE
    UPDATE_DISCONNECTED TRUE
)

FetchContent_MakeAvailable(stb)

# Link libraries
target_link_libraries(Engine       
    PRIVATE
        glm
        glfw
        Vulkan::Vulkan
        tinyobjloader
)

# Include directories for stb_image
target_include_directories(Engine
    PUBLIC
        "${stb_SOURCE_DIR}"
)

# Set properties for dependencies
set_target_properties(glm PROPERTIES FOLDER "Engine/Vendor/Public")
set_target_properties(glfw update_mappings PROPERTIES FOLDER "Engine/Vendor/Private")

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

add_custom_command(TARGET Engine POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_CURRENT_SOURCE_DIR}/models" 
    "${CMAKE_CURRENT_BINARY_DIR}/models"
    COMMENT "Copying models directory to binary directory")

file(GLOB_RECURSE GLSL_SOURCE_FILES CONFIGURE_DEPENDS
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    SOURCES ${GLSL_SOURCE_FILES}
)

# Add shader compilation as a dependency to Engine target
add_dependencies(Engine Shaders)
