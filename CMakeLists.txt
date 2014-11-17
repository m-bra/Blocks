cmake_minimum_required(VERSION 2.8.4)

project(blocks)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb -Og")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

#GLL++
include_directories(external/GLL++/GLL.cpp)

#OpenGL
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

#glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_PATH})
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${GLEW_LIBRARY})

#glm
set(GLM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
include_directories(${GLM_INCLUDE_DIRS})

#bullet
find_package(Bullet REQUIRED)
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${BULLET_LIBRARIES})
include_directories(${BULLET_INCLUDE_DIRS})

#some thread lib
find_package (Threads)
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${CMAKE_THREAD_LIBS_INIT})

#sfml
find_package(SFML REQUIRED COMPONENTS system window graphics)
include_directories(SFML_INCLUDE_DIR)
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${SFML_LIBRARIES})

add_executable(blocks main.cpp external/GLL++/Program.cpp external/GLL++/GLL.cpp)
target_link_libraries(blocks ${BLOCKS_LIBS})

install(TARGETS blocks DESTINATION bin)
install(FILES chunk.png DESTINATION res)
install(DIRECTORY shaders DESTINATION res)

include (InstallRequiredSystemLibraries)
include (CPack)
