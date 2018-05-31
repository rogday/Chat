cmake_minimum_required(VERSION 3.11.2)

project(server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_GLIBCXX_DEBUG -Wall -Wextra -O2 -std=c++17")

find_package(Boost 1.66 COMPONENTS system)
find_package (Threads)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

execute_process(COMMAND mysql_config --cflags
    OUTPUT_VARIABLE MYSQL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs
    OUTPUT_VARIABLE MYSQL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)


if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(server ${SOURCES})
  target_compile_options(server PUBLIC ${MYSQL_CFLAGS})
  target_link_libraries(server ${Boost_LIBRARIES})
  target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(server ${MYSQL_LIBS})
endif()