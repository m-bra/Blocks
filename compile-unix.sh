#!/bin/bash
set -e

cd src

cd external

if [ ! -d "GLEW-build" ]; then
    rm -r -f GLEW-install
    echo "--- Compiling GLEW ---"
    mkdir GLEW-build
    cd GLEW-build
    cmake ../GLEW -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=../GLEW-install
    make -j
    cd ..
fi

if [ ! -d "GLEW-install" ]; then
    echo "--- Installing GLEW ---"
    cd GLEW-build
    make install
    cd ..
fi

# BulletPhysics
if [ ! -d "bullet-build" ]; then
    rm -r -f bullet-install
    echo "--- Compiling Bullet ---"
    mkdir bullet-build
    cd bullet-build
    cmake ../bullet -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=../bullet-install -DBUILD_EXTRAS=OFF -DBUILD_DEMOS=OFF
    make -j
    cd ..
fi

if [ ! -d "bullet-install" ]; then
    echo "--- Installing Bullet ---"
    cd bullet-build
    make install
    cd ..
fi

# SFML
if [ ! -d "SFML-build" ]; then
    rm -r -f SFML-install
    echo "--- Compiling SFML ---"
    mkdir SFML-build
    cd SFML-build
    cmake ../SFML -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=../SFML-install
    make -j
    cd ..
fi

if [ ! -d "SFML-install" ]; then
    echo "--- Installing SFML ---"
    cd SFML-build
    make install
    cd ..
fi

cd ..

# The game!
mkdir -p build
cd build
cmake .. -G"Unix Makefiles"
make

cp blocks ../../

echo "--- Compiling completed without errors. ---"
