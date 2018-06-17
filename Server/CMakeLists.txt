cmake_minimum_required(VERSION 3.11.2)

project(server)

add_subdirectory(thirdparty/SQLiteCpp)
add_subdirectory(../API API)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_GLIBCXX_DEBUG -Wall -Wextra -O2 -std=c++17")

file(GLOB SOURCES "src/*.cpp")
find_package(Boost 1.66 COMPONENTS system)

include_directories("include" ${Boost_INCLUDE_DIRS})
add_executable(server ${SOURCES})
target_link_libraries(server ${Boost_LIBRARIES} SQLiteCpp sqlite3 pthread dl API)