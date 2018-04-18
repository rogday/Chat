#!/bin/bash
g++ -Wall -Wextra -O2 -lboost_system -lpthread $(dirname $0)/{Server.cpp,main.cpp} -o server
