cmake_minimum_required(VERSION 3.26)

project(DA_Project)

set(CMAKE_CXX_STANDARD 17)

add_executable(da_project src/main.cpp src/graph.cpp src/util.cpp src/dataParser.cpp)
