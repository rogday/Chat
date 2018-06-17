cmake_minimum_required(VERSION 3.11.2)

project(API)

set(CMAKE_CXX_FLAGS 
	"${CMAKE_CXX_FLAGS} -g -D_GLIBCXX_DEBUG -Wall -Wextra -O2 -std=c++17")

file(GLOB SOURCES "src/*.cpp")
add_library(API STATIC ${SOURCES})
target_include_directories(API PUBLIC "include")