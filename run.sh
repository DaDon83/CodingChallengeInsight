#!/bin/bash
g++ ./src/sessionization.cpp ./src/dbLinkedList.cpp --std=c++11 -o sessionization
./sessionization ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt
