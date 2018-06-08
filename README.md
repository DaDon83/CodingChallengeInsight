# CodingChallengeInsight

# Table of Contents

1. [Introduction](README.md#introduction)

2. [How to Compile](README.md#how-to-compile)

3. [Layout of the Program](README.md#layout-of-the-program)

4. [Benefits of the Data Structures](README.md#benefits-of-the-data-structures)

5. [Further Issues](README.md#further-issues)

# Introduction

This is the repository of a program written by Don Vernekohl for the Coding Challenge of the Data Engineering program from Insight. The source code folder includes the source and header files for the C++ program to solve the Edgar-Analytics Challenge. The abbreviation EDGAR stands for the Electronic Data and Gathering, Analysis and Retrieval system which was introduced by the Security and Exchange Commission (SEC). The aim is to write out inactive sessions in "real time" and to provide information about the duration of the session and the number of requested files.  

# How to Compile

The code uses the g++ compiler with standard libraries. A few functions require the C++11 standard libraries. The two source files "dbLinkedList.cpp" and "sessionization.cpp" require the header file "dbLinkedList.h" in the "src" folder.
It compiles via terminal with:

	g++ ./src_v2/sessionization.cpp ./src_v2/dbLinkedList.cpp --std=c++11 -o sessionization

I used g++ version 5.5.0. The executable is sessionization. Run the excecutables with the following command:

	./sessionization ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt

with the respective input and output files in their folders.

Three different tests are available in the "insight_testsuite" folder. Enter the folder and run "run_tests.sh" for validation.

# Layout of the Program

The problem is solved by combining a double linked list with a hash table. The hash table stores the node pointers for different IP addresses. When an "already seen" IP address appears in the new line, the pointer is used to access the respective node. The node fields are then updated and moved to the head. The node goes to the head as the head is the last node which will be accessed by the algorithm. As the node received the latest timestamp, this is exactly what we want (Process this node at the very end).
In a second step the timestamp of the input line is used to remove inactive sessions from the tail of the linked list. The algorithm calculates the time difference between the current timestamp and the latest timestamp of the last node in the linked list. If this time is larger than the inactive period, the content of the node is printed out.

After all input lines are read, the residual list is is transferred to a vector. A secon vector of integer values is assigned the start time of each node. The entries with the earliest are then written out.

# Benefits of the Data Structures

The major benefit of the linked list is that we can access old elements from the back and we can stop the search when the inactive_period is larger than the last time. If a vector or a hash table were used in this case, all elements would have to be accessed for each line to find the inactive sessions. Thus, the processing time for the linked list is much lower with the linked list. 
The hash table allows finding the current node instantly based on its IP value key which is an advantage in comparison to other data structures.

# Further Issues

(Further issues solved in src_v2 folder)
- The date processing needs to be upgraded to identify the different length of the months. The start year should be retrieved from the first line. This value is required to create a general purpose program.

- A vector of strings is used at the end of the file to empty the buffer/Linked List. It is very inefficient to remove elements from the vector which are not at the end. Hence for very large buffers, it is better to use a linked list or a hash table.

- The read_line function sorts the output vector when the required fields are not in order. In the base case, the fields are in order and there is no problem. If unordered fields are expected it would be a benefit if the read_header function determines the order of the fields instead of the read_line function.

- Issue with v2: sort after reading the file is based on time and not date information
