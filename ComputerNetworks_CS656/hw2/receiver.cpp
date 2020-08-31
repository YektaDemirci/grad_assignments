#include <iostream>
#include <cstring>
#include <string>
#include <math.h>
#include <fstream>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class packet
{
    public:
        packet(int, int, int, std::string);
        int type,seqnum,length;
        std::string data;
        void reset();
};

packet::packet(int type_def=0, int seqnum_def=0, int length_def=0, std::string data_def="")
{
    type=type_def;
    seqnum=seqnum_def;
    length=length_def;
    data=data_def;
}

void packet::reset()
{
    type=0;
    seqnum=0;
    length=0;
    data.clear();
}

int main(int argc, char** argv)
{
    std::ofstream myfile;
    std::ofstream logfile;
    myfile.open(argv[4]);
    logfile.open("arrival.log");
    int rec_port=0;
    int send_port=0;

    /* Char to int converter */
    int ctr=0;
    int temp=0;
    while(argv[3][ctr] != '\0')
        ctr++;
    int digit=(ctr-1);
    for(int i=0; i<ctr;i++)
    {
        temp = (int) argv[3][digit];
        rec_port= ( (temp-48)*pow (10,i) ) + rec_port;  
        digit--; 
    }

    ctr=0;
    temp=0;
    while(argv[2][ctr] != '\0')
        ctr++;
    digit=(ctr-1);
    for(int i=0; i<ctr;i++)
    {
        temp = (int) argv[2][digit];
        send_port= ( (temp-48)*pow (10,i) ) + send_port;  
        digit--; 
    }
    

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    {
        perror("Error:Opening the listening socket");
        exit(1);
    }

    bzero((char *) &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(rec_port);

    if((bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0))
    {
        std::cerr << "The receiver listening UDP port is not available \n";
    }

    /* packet object size*/
    /* Data size */
    unsigned char buff_rec[512];
    unsigned char buff_ACK[512];

    struct sockaddr_in clientaddr;
    int len = sizeof(clientaddr);
    int cntr=0;
    std::cout << "Listening packets" << std::endl;

    int sockACK;
    struct sockaddr_in ACKaddr;
    sockACK = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockACK < 0) 
    {
        perror("Error:Opening the ACK socket");
        exit(1);
    }
    bzero((char *) &ACKaddr, sizeof(ACKaddr));
    /* IPv4 */
    ACKaddr.sin_family = AF_INET;
    /* IP of the receiver */
    ACKaddr.sin_addr.s_addr = inet_addr(argv[1]);
    ACKaddr.sin_port = htons(send_port);

    packet dummy;
    int ctr_out;
    int seqCntrl=31;

    while(1)
    {
        dummy.reset();
        /* packet object size*/
        /* Data size */
        bzero(buff_rec,512);
        recvfrom(sockfd,buff_rec, sizeof(buff_rec),0,(struct sockaddr *)&clientaddr,(socklen_t*)&len);
        /*Byte to int conversions*/
        dummy.type=0;
        dummy.type = dummy.type+(int)(buff_rec[3] );
        dummy.type = dummy.type+(int)(buff_rec[2] << 8);
        dummy.type = dummy.type+(int)(buff_rec[1] << 16);
        dummy.type = dummy.type+(int)(buff_rec[0] << 24);

        dummy.seqnum=0;
        dummy.seqnum = dummy.seqnum+(int)(buff_rec[7] );
        dummy.seqnum = dummy.seqnum+(int)(buff_rec[6] << 8);
        dummy.seqnum = dummy.seqnum+(int)(buff_rec[5] << 16);
        dummy.seqnum = dummy.seqnum+(int)(buff_rec[4] << 24);

        dummy.length=0;
        dummy.length = dummy.length+(int)(buff_rec[11] );
        dummy.length = dummy.length+(int)(buff_rec[10] << 8);
        dummy.length = dummy.length+(int)(buff_rec[9] << 16);
        dummy.length = dummy.length+(int)(buff_rec[8] << 24);

        ctr_out=12;
        if( ((seqCntrl+1)%32) == dummy.seqnum)
        {
            while(buff_rec[ctr_out] != '\0')
            {
                myfile  << buff_rec[ctr_out];
                ctr_out++;
            }
            seqCntrl++;
            seqCntrl=seqCntrl%32;

        }
        logfile << dummy.seqnum << '\n';
                
        //std::cout << "Received packet length is " << dummy.length << std::endl;
        for(int i=12;i<12+dummy.length;i++)
        {
            dummy.data.push_back(buff_rec[i]);
        }
        std::cout <<" type: " << dummy.type << " seqNum: " << dummy.seqnum << " length: " << dummy.length << std::endl;
        
        packet ACK;
        ACK.type=0;
        ACK.seqnum=seqCntrl;
        ACK.length=0;
        /* packet object size*/
        /* Data size */
        bzero(buff_ACK,512);
        buff_ACK[0] = (ACK.type >>24);
        buff_ACK[1] = (ACK.type>>16);
        buff_ACK[2] = (ACK.type>>8);
        buff_ACK[3] = (ACK.type);

        buff_ACK[4] = ACK.seqnum>>24;
        buff_ACK[5] = ACK.seqnum>>16;
        buff_ACK[6] = ACK.seqnum>>8;
        buff_ACK[7] = ACK.seqnum;

        buff_ACK[8] = ACK.length >>24;
        buff_ACK[9] = ACK.length>>16;
        buff_ACK[10] = ACK.length>>8;
        buff_ACK[11] = ACK.length;

        if (connect(sockACK, (struct sockaddr *)&ACKaddr, sizeof(ACKaddr)) != 0)
        { 
            printf("Connection with the sender failed\n"); 
            exit(0); 
        }
        
        if(dummy.type == 2)
        {
            bzero(buff_ACK,512);
            ACK.type=2;

            buff_ACK[0] = (ACK.type >>24);
            buff_ACK[1] = (ACK.type>>16);
            buff_ACK[2] = (ACK.type>>8);
            buff_ACK[3] = (ACK.type);

            buff_ACK[4] = ACK.seqnum>>24;
            buff_ACK[5] = ACK.seqnum>>16;
            buff_ACK[6] = ACK.seqnum>>8;
            buff_ACK[7] = ACK.seqnum;

            sendto(sockACK, buff_ACK, sizeof(buff_ACK), 0, (struct sockaddr*)NULL, sizeof(ACKaddr));

            logfile.close();
            myfile.close();
            break;
        }
        else
        {
            sendto(sockACK, buff_ACK, sizeof(buff_ACK), 0, (struct sockaddr*)NULL, sizeof(ACKaddr));
        }
    }
    std::cout << "Receiver is off" << std::endl;
    return 0;
}