#!/bin/bash
g++ -Wall -Wextra -march=native -O2 -lboost_system -lpthread server.cpp -o server
