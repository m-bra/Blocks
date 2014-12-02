cmake_minimum_required(VERSION 2.8.4)

project(blocks)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

# GLL++
include_directories(${CMAKE_SOURCE_DIR}/external/GLL++)

# glm
set(GLM_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/external/glm")
include_directories(${GLM_INCLUDE_DIRS})

# OpenGL
find_package(OpenGL)
if(!OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL required.")
endif(!OPENGL_FOUND)
if(OPENGL_GLU_FOUND)
    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${OPENGL_LIBRARIES})
else(OPENGL_GLU_FOUND)
    message(WARNING "GLU not found.")
    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${OPENGL_gl_LIBRARIES})
endif(OPENGL_GLU_FOUND)
include_directories(${OPENGL_INCLUDE_DIR})

# some thread lib
find_package (Threads)
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${CMAKE_THREAD_LIBS_INIT})

# glew
if (WIN32)
  message (FATAL_ERROR Windws not supported yet)
else (WIN32)
  include_directories(external/GLEW-install/include)
  set(BLOCKS_STATIC_LIBS ${BLOCKS_LIBS} ${CMAKE_SOURCE_DIR}/external/GLEW-install/libglew.a)
endif(WIN32)

# bullet
set(BULLET_ROOT external/bullet-install)
find_package(Bullet)
if (BULLET_FOUND)
    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${BULLET_LIBRARIES})
    include_directories(${BULLET_INCLUDE_DIRS})
else (BULLET_FOUND)
    message(FATAL_ERROR "BulletPhysics not found. Did you use the installers?")
endif (BULLET_FOUND)

# sfml
set(SFML_ROOT external/SFML-install)
find_package(SFML COMPONENTS system window graphics)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${SFML_LIBRARIES})
else (SFML_FOUND)
    message(FATAL_ERROR "SFML not found. Did you use the installers?")
endif (SFML_FOUND)

add_executable(blocks main.cpp external/GLL++/Program.cpp external/GLL++/GLL.cpp)
target_link_libraries(blocks ${BLOCKS_LIBS})
