#!/bin/bash
g++ ./src_v2/sessionization.cpp ./src_v2/dbLinkedList.cpp --std=c++11 -o sessionization
./sessionization ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt
