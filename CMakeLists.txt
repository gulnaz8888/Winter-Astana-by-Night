cmake_minimum_required(VERSION 3.16)
project(WinterAstana CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(winter ${SOURCES} external/glad/src/glad.c)

target_include_directories(winter PRIVATE src external/glad/include external/stb)
target_link_libraries(winter PRIVATE glfw glm::glm)
target_compile_definitions(winter PRIVATE GL_SILENCE_DEPRECATION)

add_custom_command(TARGET winter POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:winter>/shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets  $<TARGET_FILE_DIR:winter>/assets)
