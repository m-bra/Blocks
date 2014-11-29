#!/bin/bash
set -e

cd src

cd external

# BulletPhysics
if [ ! -d "bullet-build" ]; then
  echo Compiling Bullet...
  mkdir bullet-build
  cd bullet-build
  cmake ../bullet -G "Unix Makefiles" -DBUILD_EXTRAS=OFF -DBUILD_DEMOS=OFF
  make -j
  cd ..
fi

# SFML
if [ ! -d "SFML-build" ]; then
  echo Compiling SFML...
  mkdir SFML-build
  cd SFML-build
  cmake ../SFML -G "Unix Makefiles"
  make -j
  cd ..
fi

cd ..

# The game!
mkdir -p build
cd build
cmake .. -G"Unix Makefiles"
make

cp blocks ../../

echo "Compiling completed without errors."
