cmake_minimum_required(VERSION 3.9)
project(FinalPackerTry)

set(CMAKE_CXX_STANDARD 11)

add_executable(FinalPackerTry main.cpp HelperClasses/Compressor.cpp HelperClasses/Compressor.h HelperClasses/Allocator.cpp HelperClasses/Allocator.h HelperClasses/CodePatcher.cpp HelperClasses/CodePatcher.h HelperClasses/Globals.h HelperClasses/Globals.cpp)

add_executable(loader-test tests/testLoader.c)

# Loaders
add_compile_options(-fPIC)
add_executable(write-loaders LoadersCode/writeLoaders.c)
