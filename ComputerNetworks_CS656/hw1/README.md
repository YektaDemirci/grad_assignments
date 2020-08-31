# Explanation
### Introduction
In this assignment, the client will send requests to the server to reverse strings (taken as a command line input) over the network using sockets. \
This assignment uses a two stage communication process. In the negotiation stage, the client and the
server negotiate on a random port (<r_port>) for later use through a fixed negotiation port (<n_port>)of the server. Later in the transaction stage, the client connects to the server through the selected random port for actual data transfer.
### How to compile
Compiled with g++ ,c++11. To compile and obtain the executables just type\
$make\
Then you can run server.sh and client.sh
### Executables and inputs
There are two executables server and client. The command line inputs should be given as it is explained in assigment1 handout. Otherwise code throws an error. Input format is also checked, if the format is not proper, it throws an error with explanation.\

$./server.sh <req_code> \
$./client.sh <server_addr> <n_port> <req_code> <msg_to_reverse>
  
### The machines used for test
The client and server were both tested on `ubuntu1804-002`.Then server was tested on `ubuntu1804-002` and client was tested on  `ubuntu1804-004`, given machines at https://cs.uwaterloo.ca/cscf/internal/infrastructure/inventory/CS-TEACHING/hosts
### Github link
https://github.com/YektaDemirci/CS656-assg1
## Thanks!
Yekta
