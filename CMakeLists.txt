cmake_minimum_required(VERSION 3.16)

project(uniq_res)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)
