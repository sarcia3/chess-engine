cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chessengine VERSION 1.0)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(chessengine main.cpp ${SRC_FILES})
target_include_directories(chessengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
