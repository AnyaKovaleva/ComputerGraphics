cmake_minimum_required(VERSION 3.23)
project(ComputerGraphics)

set(CMAKE_CXX_STANDARD 14)

add_executable(ComputerGraphics "Source Files/3dView.cpp")

target_link_libraries(ComputerGraphics -lOpenGL32 -lfreeGLUT)

#include_directories("Libraries/include")
#include_directories("Dependencies")
#
#add_library(glfw STATIC IMPORTED)
#set_target_properties(glfw PROPERTIES
#        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib-vc2019/glfw3.lib"
#        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/include")
#target_link_libraries(ComputerGraphics glfw)
