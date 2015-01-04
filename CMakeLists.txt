cmake_minimum_required(VERSION 2.8.4)

project(blocks)

#set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# only on my machine! :)
if (IS_DIRECTORY /home/mbra)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -g")
    message("Enabling debugging! Might not work everywhere! To turn off, remove this!")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

# GLL++
include_directories(${CMAKE_SOURCE_DIR}/external/GLL++)

# glm
set(GLM_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/external/glm")
include_directories(${GLM_INCLUDE_DIRS})

# OpenGL
#find_package(OpenGL)
#if(!OPENGL_FOUND)
#    message(FATAL_ERROR "OpenGL required.")
#endif(!OPENGL_FOUND)
#if(OPENGL_GLU_FOUND)
#    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${OPENGL_LIBRARIES})
#else(OPENGL_GLU_FOUND)
#    message(WARNING "GLU not found.")
#    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${OPENGL_gl_LIBRARIES})
#endif(OPENGL_GLU_FOUND)
#include_directories(${OPENGL_INCLUDE_DIR})

# some thread lib
find_package (Threads REQUIRED)
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${CMAKE_THREAD_LIBS_INIT})

# bullet
set(BULLET_ROOT ../build/bullet)
find_package(Bullet REQUIRED)
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${BULLET_LIBRARIES})
include_directories(${BULLET_INCLUDE_DIRS})

# glfw
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glfw3 ../glfw3)
include_directories(${CMAKE_SOURCE_DIR}/external/glfw3/include)
set(BLOCKS_LIBS ${BLOCKS_LIBS} glfw ${GLFW_LIBRARIES} ${OPENGL_glu_LIBRARY})
message("GLFW LIBRARIES: ${GLFW_LIBRARIES}")

# glew
#if (WIN32)
#  message (FATAL_ERROR Windows not supported yet)
#else (WIN32)
#  include_directories(../glew/include)
#  set(BLOCKS_LIBS ${BLOCKS_LIBS} ../glew/lib/libglew.a)
#endif(WIN32)
set(GLEW_ROOT_DIR ../build/glew)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_PATH})
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${GLEW_LIBRARY})

file(GLOB_RECURSE BLOCKS_SRC src/*.cpp)
include_directories(src)

add_executable(blocks ${BLOCKS_SRC} external/GLL++/Program.cpp external/GLL++/GLL.cpp)
target_link_libraries(blocks ${BLOCKS_LIBS})
