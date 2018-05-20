#!/bin/bash
g++ -std=c++17 -D_GLIBCXX_DEBUG -Wall -Wextra -O2 $(dirname $0)/{Server,Room,main,Client}.cpp -o server -lboost_system -lpthread
