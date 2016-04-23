#!/bin/bash
set -e

mkdir -p build
cd build

echo "--- Compiling GLEW ---"
cp -r ../external/glew glew
cd glew
make
cd ..

echo "--- Compiling Bullet ---"
mkdir -p bullet; cd bullet
cmake ../../external/bullet -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=. -DBUILD_EXTRAS=OFF -DBUILD_DEMOS=OFF
make && make install
cd ..

echo "--- Compiling Blocks ---"
mkdir -p Blocks
cd Blocks
cmake ../.. -G "Unix Makefiles"
make

cp blocks ../../

echo "--- Compilation terminated succesfully ---"
