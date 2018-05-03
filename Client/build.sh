#!/bin/bash
g++ -Wall -Wextra -O2 -lboost_system -lpthread $(dirname $0)/Client.cpp -o client
