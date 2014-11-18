cmake_minimum_required(VERSION 2.8.4)

project(blocks)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

# GLL++
include_directories(${CMAKE_SOURCE_DIR}/external/GLL++)

#glm
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

#some thread lib
find_package (Threads)
set(BLOCKS_LIBS ${BLOCKS_LIBS} ${CMAKE_THREAD_LIBS_INIT})

#glew
option(DOWNLOAD_GLEW "Whether to download GLEW when it is not found (needs git)" NO)
find_package(GLEW)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_PATH})
    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${GLEW_LIBRARY})
elseif (DOWNLOAD_GLEW)
    message("Downloading GLEW from github...")
    execute_process(COMMAND git clone https://github.com/omniavinco/glew-cmake.git ${CMAKE_SOURCE_DIR}/external/glew/)
elseif (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/external/glew/")
    message("Found glew in external/glew")
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/glew/)
else ()
    message(FATAL_ERROR "GLEW not found. Please install it, or set DOWNLOAD_GLEW to YES to let me download it.")
endif(GLEW_FOUND)

#bullet
option(DOWNLOAD_BULLET "Whether to download BulletPhysics when it is not found (needs git)" NO)
find_package(Bullet)
if (BULLET_FOUND)
    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${BULLET_LIBRARIES})
    include_directories(${BULLET_INCLUDE_DIRS})
elseif (DOWNLOAD_BULLET)
    message("Downloading BULLET from github...")
    execute_process(COMMAND git clone https://github.com/bulletphysics/bullet3.git ${CMAKE_SOURCE_DIR}/external/bullet)
elseif (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/external/bullet/")
    message("Found bullet in external/bullet")
    set(USE_GLUT NO)
    set(BUILD_BULLET2_DEMOS NO)
    set(BUILD_CPU_DEMOS NO)
    set(BUILD_OPENGL3_DEMOS NO)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/bullet/)
else (BULLET_FOUND)
    message(FATAL_ERROR "BulletPhysics not found. Please install it, or set DOWNLOAD_BULLET to YES to let me download it.")
endif (BULLET_FOUND)

#sfml
option(DOWNLOAD_SFML "Whether to download SFML when it is not found (needs git)" NO)
find_package(SFML COMPONENTS system window graphics)
if (SFML_FOUND)
    include_directories(SFML_INCLUDE_DIR)
    set(BLOCKS_LIBS ${BLOCKS_LIBS} ${SFML_LIBRARIES})
elseif (DOWNLOAD_SFML)
    message("Downloading SFML from github...")
    execute_process(COMMAND git clone https://github.com/LaurentGomila/SFML.git ${CMAKE_SOURCE_DIR}/external/sfml)
elseif (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/external/sfml/")
    message("Found sfml in external/sfml")
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/sfml/)
else (SFML_FOUND)
    message(FATAL_ERROR "SFML not found. Please install it, or set DOWNLOAD_SFML to YES to let me download it.")
endif (SFML_FOUND)

add_executable(blocks main.cpp external/GLL++/Program.cpp external/GLL++/GLL.cpp)
target_link_libraries(blocks ${BLOCKS_LIBS})

install(TARGETS blocks DESTINATION bin)
install(FILES chunk.png DESTINATION res)
install(DIRECTORY shaders DESTINATION res)

include (InstallRequiredSystemLibraries)
include (CPack)
