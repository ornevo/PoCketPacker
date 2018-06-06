cmake_minimum_required(VERSION 3.9)
project(FinalPackerTry)

set(CMAKE_CXX_STANDARD 11)

add_executable(FinalPackerTry main.cpp ELFHandlers/Utility.h ELFHandlers/Utility.cpp Processors/Compressor.cpp Processors/Compressor.h ELFHandlers/Allocator.cpp ELFHandlers/Allocator.h ELFHandlers/CodePatcher.cpp ELFHandlers/CodePatcher.h Helpers/Globals.h Helpers/Globals.cpp)

add_executable(loader-test tests/testLoader.c)

# Loaders
add_compile_options(-fPIC)
add_executable(write-loaders LoadersCode/writeLoaders.c)
