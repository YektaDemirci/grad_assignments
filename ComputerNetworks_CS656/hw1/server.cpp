#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

/*For port.txt*/
#include <fstream>
/*For fork() etc*/
#include<unistd.h>
/*For cout() etc*/
#include <iostream>
/*For pow etc*/
#include <math.h>
/*To check input formar*/
#include <regex>

/*Negotiation part, if the correct code is received UDP socket is set up */
int negotiation (int sock,int req_code_original) {

    char buffer[1024];

    bzero(buffer,1024);
    read(sock,buffer,1023);

    int sockUDP=-1;
    int req_code_received = atoi(buffer);
    /*If the correct code is received UDP is set up*/
    if(req_code_original == req_code_received)
    {
        
        int portno_UDP;
        /* Second connection is UDP(SOCK_STREAM), IPv4 Protocol(AF_INET), the rest default(0) */
        sockUDP = socket(AF_INET, SOCK_DGRAM, 0);

        struct sockaddr_in serv_addr_UDP;

        bzero((char *) &serv_addr_UDP, sizeof(serv_addr_UDP));
        portno_UDP = 2103;

        /* IPv4 */
        serv_addr_UDP.sin_family = AF_INET;
        /* IP of the machine */
        serv_addr_UDP.sin_addr.s_addr = INADDR_ANY;
        /* Lets try the port */
        serv_addr_UDP.sin_port = htons(portno_UDP);

        /*Brute force socket finding*/
        while (bind(sockUDP, (struct sockaddr *) &serv_addr_UDP, sizeof(serv_addr_UDP)) < 0)
        {
            portno_UDP = portno_UDP+1;
            serv_addr_UDP.sin_port = htons(portno_UDP);
            //std::cout << portno_UDP << std::endl;
        }
        /*Number of digits in the socket is found, it is probably 4 digit still hard coded*/
        int digit=0;
        int dummyPort=portno_UDP;
        while(dummyPort>0)
        {
            digit++;
            dummyPort=dummyPort/10;
        }
        bzero(buffer,1024);
        dummyPort=portno_UDP;
        /*UDP socket is converted from int to char array*/
        while(digit>0)
        {
           digit=digit-1;
           buffer[digit]='0'+(dummyPort%10);
           dummyPort=dummyPort/10;
        }
        /*UDP socket is sent to client*/
        write(sock,buffer,1024);
        return sockUDP;
    }
    /*Wrong <req_code> so 'f' is sent, so client can exit instead of waiting for a socket number*/
    else
    {
        bzero(buffer,1024);
        buffer[0]='f';
        write(sock,buffer,1024);
        std::cerr<<"Error:Received <req_code> is wrong \n";
        return sockUDP;
    }
    
}
/*Where the input array is reversed*/
void transaction(int socket)
{
    struct sockaddr_in clientaddr;
    int len = sizeof(clientaddr);

    char buff[1024];
    bzero(buff,1024);
    while(buff[0]=='\0')
    {
        recvfrom(socket, buff, sizeof(buff), 0, (struct sockaddr *)&clientaddr,(socklen_t*)&len); 
    }

    int head = 0;
    int tail;

    char * dummy;
    dummy = strchr(buff, '\0');
    tail = (int)(dummy - buff);
    tail=tail-1;
    /*Head shows the first index, tail is the last index, characters are swapped so the message is reversed*/
    char dummy2;
    while(tail>head)
    {
        dummy2=buff[head];
        buff[head]=buff[tail];
        buff[tail]=dummy2;

        head=head+1;
        tail=tail-1;
    }
    sendto(socket, buff, sizeof(buff), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
}


int main( int argc, char *argv[] ) 
{
    /*Number of inputs are checked*/
    if(argc != 2)
    {
        std::cerr << "Error:Wrong input format,try $./server <req_code> \n";
        exit(1);
    }
    else
    {
        /*<req_code> format is checked*/
        std::regex r("[[:digit:]]+");

        if(regex_match(argv[1],r))
        {

            int sockfd, newsockfd, portno, clilen;
            struct sockaddr_in serv_addr, cli_addr;
            int pid;

            int req_code;
            req_code=atoi(argv[1]);

            /* First connection is TCP(SOCK_STREAM), IPv4 Protocol(AF_INET), the rest default(0) */
            sockfd = socket(AF_INET, SOCK_STREAM, 0);

            /* Initialize socket structure */
            /* serv_addr is cleaned */
            bzero((char *) &serv_addr, sizeof(serv_addr));
            portno = 2102;

            /* IPv4 */
            serv_addr.sin_family = AF_INET;
            /* IP of the machine */
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            /* Lets try the port */
            serv_addr.sin_port = htons(portno);
        
        /* Now bind the host address using bind() call.*/
            while (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            {
                portno = portno+1;
                serv_addr.sin_port = htons(portno);
                //std::cout << portno << std::endl;
            }
            std::ofstream outputFile;
            outputFile.open("port.txt");
            outputFile << portno << std::endl;
            outputFile.close();

            std::cout <<"SERVER_PORT="<<portno << std::endl;
        
            /* Lets listen, size of the queue is 5 eventhough only 1 will be used */
            listen(sockfd,5);
            clilen = sizeof(cli_addr);
        
            while (1) 
            {
                /* Adres of the client is saved */
                /* New socket is assigned */
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
                
                /* Create child process so server can keep listening */
                pid = fork();
                
                if (pid == 0) 
                {
                    /* This is the client process */
                    close(sockfd);
                    int sock_UDP;
                    sock_UDP=negotiation(newsockfd,req_code);
                    close(newsockfd);

                    if(sock_UDP>0)
                    {
                        transaction(sock_UDP);
                        close(sock_UDP);
                        exit(0);
                    }
                    else
                    {
                        exit(0);
                    }
                }
                else 
                {
                    close(newsockfd);
                }	
            }/*End of while*/
        }/*End of regex check*/
        else
        {
            std::cerr<< "Error:Command line input must be integer \n";
            exit(1);
        }
    }/*End of command line check*/
}

