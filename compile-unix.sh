#!/bin/bash
set -e

mkdir -p build
cd build

echo "--- Compiling GLEW ---"
mkdir -p GLEW; cd GLEW
cmake ../../src/external/GLEW -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=.
make -j && make install
cd ..

echo "--- Compiling Bullet ---"
mkdir -p bullet; cd bullet
cmake ../../src/external/bullet -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=. -DBUILD_EXTRAS=OFF -DBUILD_DEMOS=OFF
make -j && make install
cd ..

echo "--- Compiling SFML ---"
mkdir -p SFML; cd SFML
cmake ../../src/external/SFML -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=.
make -j && make install
cd ..

echo "--- Compiling Blocks ---"
mkdir -p Blocks
cmake ../src -G "Unix Makefiles"
make -j

cp blocks ../../

echo "--- Compilation terminated succesfully ---"
