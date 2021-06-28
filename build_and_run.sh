#!/bin/bash
cd ./src
echo "Executing CMake"
cmake --debug-output
echo "Executing Make"
make && ../bin/OpenSGS || echo "Failed to build, not running program."
