cmake_minimum_required(VERSION 3.10)

#set the project name
project(KEngine)

add_library(KEngineBasics STATIC
Input.h
Input.cpp
)

target_compile_features(KEngineBasics PRIVATE cxx_std_17) 
target_link_libraries(KEngineBasics PUBLIC KEngineCore KEngine2D)
target_include_directories(KEngineBasics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})