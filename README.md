# README

### 1. What is this?

A minimal application for closing applications that shows a simple exit menu, using
UIA.

### 2. Usage (dev)

Setup and use as a Conan dependency:

conan remote add Apps https://bintray.com/javjarfer/Apps
conan install "CloseApplication/0.1@javjarfer/testing"

#### Building

In project folder:

For debugging purposes:

1. mkdir build && cd build
2. conan install .. -s build_type=Debug  --build=missing -s compiler="Visual Studio" -s compiler.runtime="MDd"
3. cmake -G "Visual Studio 15 Win64" -DCMAKE_BUILD_TESTS=ON ..
4. cmake --build . --target ALL_BUILD --config Debug

For building a release version:

1. mkdir build && cd build
2. conan install .. -s build_type=Release  --build=missing
3. cmake -G "Visual Studio 15 Win64" -DCMAKE_BUILD_TESTS=ON ..
4. cmake --build . --target ALL_BUILD --config Release

