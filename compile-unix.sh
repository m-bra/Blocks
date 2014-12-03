#!/bin/bash
set -e

mkdir -p build
cd build

echo "--- Compiling GLEW ---"
mkdir -p glew; cd glew
cmake ../../src/external/glew -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=.
make -j && make install
cd ..

echo "--- Compiling Bullet ---"
mkdir -p bullet; cd bullet
cmake ../../src/external/bullet -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=. -DBUILD_EXTRAS=OFF -DBUILD_DEMOS=OFF
make -j && make install
cd ..

# Done in src/CMakeLists.txt with add_subdirectory
#echo "--- Compiling GLFW ---"
#mkdir -p glfw3; cd glfw3
#cmake ../../src/external/glfw3 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=.
#make -j && make install
#cd ..

echo "--- Compiling Blocks ---"
mkdir -p Blocks
cd Blocks
cmake ../../src -G "Unix Makefiles"
make -j

cp blocks ../../

echo "--- Compilation terminated succesfully ---"
