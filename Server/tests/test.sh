#!/bin/bash

g++ $(dirname $0)/genTestPacket.cpp -o $(dirname $0)/tester

$(dirname $0)/../server 6666&
PID=$!

./$(dirname $0)/tester "rogday:qwerty" "Shitty room" "Heoll, wrold!11" | nc -x 127.0.0.1 6666 1 > $(dirname $0)/log.txt 2>&1 &

sleep 2 && kill -SIGINT $PID && sleep 1 && echo -e "\nPkg:\n" && cat $(dirname $0)/log.txt
