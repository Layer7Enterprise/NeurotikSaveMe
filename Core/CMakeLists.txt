cmake_minimum_required(VERSION 2.6.4)
project(NeurotikCore)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
add_executable(neurotik main.cpp Net.cpp Schema.cpp GetNet.cpp)
