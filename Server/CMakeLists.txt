cmake_minimum_required(VERSION 3.11.2)

project(server)

add_subdirectory(thirdparty/SQLiteCpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_GLIBCXX_DEBUG -Wall -Wextra -O2 -std=c++17")

find_package(Boost 1.66 COMPONENTS system)

include_directories("include" "../API/include")
file(GLOB SOURCES "src/*.cpp" "../API/src/*.cpp")

include_directories(${Boost_INCLUDE_DIRS})
add_executable(server ${SOURCES})

target_link_libraries(server SQLiteCpp sqlite3 pthread dl)
target_link_libraries(server ${Boost_LIBRARIES})