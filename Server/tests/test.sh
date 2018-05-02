#!/bin/bash

g++ genTestPacket.cpp -o tester
./tester "rogday:qwerty" "Shitty room" "Heoll, wrold!11"

./server 6666&
PID=$!

cat test.pkg| nc -cx 127.0.0.1 6666 1 > log.txt 2>&1

sleep 5 && kill -SIGINT $PID && echo -e "\nPkg:\n" && cat log.txt
