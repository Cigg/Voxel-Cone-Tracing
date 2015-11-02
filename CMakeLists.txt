cmake_minimum_required(VERSION 2.8)
project(VCT)

# GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(lib/glfw)

# ASSIMP
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(lib/assimp)

# GLEW
option(glew-cmake_BUILD_SHARED "" OFF)
add_subdirectory(lib/glew)

# Set compilation flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
endif()

# Include directories
include_directories(include/
                    lib/assimp/include/
                    lib/glfw/include/
                    lib/glew/include/
                    lib/glew/src/
                    lib/glm/
                    lib/stb/)

# Generate lists of source files
file(GLOB PROJECT_HEADERS include/*.h)
file(GLOB PROJECT_SOURCES src/*.cpp)
file(GLOB PROJECT_SHADERS shaders/*.comp
                          shaders/*.frag
                          shaders/*.geom
                          shaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

# Visual studio groups
source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})

# GCCFilter, if appliciable
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCPP)
  option(COLOR_GCC "Use GCCFilter to color compiler output messages" ON)
  set(COLOR_GCC_OPTIONS "-c -r -w" CACHE STRING "Arguments that are passed to gccfilter when output coloring is switchend on. Defaults to -c -r -w.")
  if(COLOR_GCC)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "gccfilter ${COLOR_GCC_OPTIONS}")
  endif()
endif()

# Add compiler preprocessor definitions
add_definitions(-DGLEW_STATIC)
add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

# Add executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS})

# Link with libraries
target_link_libraries(${PROJECT_NAME} assimp glfw ${GLFW_LIBRARIES} libglew_static)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})