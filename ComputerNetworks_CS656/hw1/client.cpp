#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

/*For fork() etc*/
#include<unistd.h>
/*For cout() etc*/
#include <iostream>
#include <regex>

#include <arpa/inet.h>


  
int main( int argc, char *argv[] )
{
    /*Number of inputs are checked here*/
    if(argc != 5)
    {
        std::cerr << "Error:Wrong input format,try $./client <server_address> <n_port> <req_code> <msg> \n";
        exit(1);
    }
    else
    {
        /*regex checks ip is for <server_address>, r is for <n_port> and <req_code>*/
        std::regex ip("^(?:(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])(\\.(?!$)|$)){4}$");
        std::regex r("[[:digit:]]+");
        
        if(!regex_match(argv[1],ip))
        {
            std::cerr<< "Error:<server_address> must be in the IPv4 format \n";
            exit(1);
        }
        if(!regex_match(argv[2],r))
        {
            std::cerr<< "Error:<n_port> must be integer \n";
            exit(1);
        }
        if(!regex_match(argv[3],r))
        {
            std::cerr<< "Error:<req_code> must be integer \n";
            exit(1);
        }
        
        int sockfd; 
        struct sockaddr_in servaddr; 
    
        /* First connection is TCP(SOCK_STREAM), IPv4 Protocol(AF_INET), the rest default(0) */
        sockfd = socket(AF_INET, SOCK_STREAM, 0); 
        if (sockfd < 0) 
        {
            perror("Error:Opening socket");
            exit(1);
        }
        
        bzero((char *) &servaddr, sizeof(servaddr)); 
    
        /* IPv4 */
        servaddr.sin_family = AF_INET;
        /* IP of the machine */
        servaddr.sin_addr.s_addr = inet_addr(argv[1]);
        /* Lets try the port */
        servaddr.sin_port = htons(atoi(argv[2]));
    
        // connect the client socket to server socket 
        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
        { 
            std::cerr<< "Error:Connection with the server failed\n"; 
            exit(0); 
        }

        char buff[1024];
        bzero(buff,1024);
        strcpy(buff, argv[3]);
        /*<req_code> is sent to the server*/
        write(sockfd, buff, sizeof(buff)); 

        bzero(buff,1024);

        read(sockfd,buff,1023);
        /*If wrong <req_code> is sent, then server returns 'f'*/
        if(buff[0]=='f')
        {
            return 0;
        }
        /*If correect <req_code> is sent, server returns UDP socket number*/
        else
        {
            int UDP_port;
            UDP_port=atoi(buff);
            //sleep(3);
            close(sockfd);

            sockfd = socket(AF_INET, SOCK_DGRAM, 0);

            bzero((char *) &servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = inet_addr(argv[1]);
            servaddr.sin_port = htons(UDP_port);

            if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
            { 
                printf("Connection with the server failed\n"); 
                exit(0); 
            }

            bzero(buff,1024);
            strcpy(buff, argv[4]);

            /*Message is sent to the server via UDP*/
            sendto(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
            
            bzero(buff,1024);
            /*Reversed message is received via UDP*/
            while(buff[0]=='\0')
            {
                recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)NULL, NULL); 
            }
            printf("%s \n",buff);

        
            // close the UDP socket 
            close(sockfd);
            return 0;
        }/*End of req_code scenarios*/
    }/*End of command line check*/
    
} 
