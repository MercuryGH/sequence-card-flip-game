cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME "flip")

file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp include/*.hpp)

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)
target_include_directories(${TARGET_NAME} PUBLIC include)
