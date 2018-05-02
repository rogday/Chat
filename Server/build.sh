#!/bin/bash
g++ -D_GLIBCXX_DEBUG -Wall -Wextra -O2 $(dirname $0)/{Server,Room,main,Client}.cpp -o server -lboost_system -lpthread
