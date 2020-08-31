# Explanation
## Introduction
For this assignment, you will implement a shortest path routing algorithm. (OSPF). A network with 5 routers.
## How to compile
Compiled with g++ ,c++11. To compile and obtain the executables just type\
$make\
Then you can run ./router with the required inputs given in A3 handout.
## Executables and inputs
There will be one executable: router, it is assumed the inputs are given in the correct order and format
## Outputs
Each router creates its own log file. The logging is done as it is explained in A3-FAQ-flow. First the message is received, then the database is updated and logged(if the link is new), then the message is flooded(if the link is new), then RIB is logged.
## The machines used for test
'ubuntu1804-004' and 'ubuntu1804-008' were used to test the code.
### Thanks!
Yekta

