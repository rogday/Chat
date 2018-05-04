#!/bin/bash
g++ -Wall -Wextra -O2 $(dirname $0)/{Client.cpp,main.cpp} -o client -lboost_system -lpthread
