#!/bin/bash

## Create build directory if not found
if [ -d "$DIRECTORY" ]; then
  mkdir build
fi

## Invoke cmake to generate makefile
cmake -S . -B ./build

## Compile and build the executables
cmake --build build
