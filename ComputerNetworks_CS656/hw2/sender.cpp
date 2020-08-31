#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <ctime>

/* Packet class as it is given in the handout */
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

/* Some structs for the threads */
struct arg_struct
{
    std::string inp;
};

struct arg_emul
{
    std::string emulIP;
    std::string emulPort;
};

struct arg_rec
{
    std::string recPort;
};

struct arg_timer
{
    int flag;
    int initalIndex;
    int lastIndex;
    int seqNo;
    float timeBegin;
    clockid_t threadID;
};


/* Shared variables among threads */

/* Number of packets read and ready to be sent in the window */
int READ_WINDOW=0;
/* Number of packets sent but not ACKed the window */
int SENT=0;
/* ACKed highest seq number */
int SEQ_ACKED = 0;
/* Base index in  the window */
int SENDER_WINDOW_INDX=0;

/* The above variables will be accessed by multiple threads so mutex is needed*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* To exit threads */
int FINISH_FLAG_1=0;
int FINISH_FLAG_2=0;

/* window size */
packet the_window[10];

/* To measure timeout */
arg_timer timerVar;

/* This threads reads the data from txt as window moves */
void *readText(void *arguments)
{
    std::cout << "Reading thread started " << std::endl;
    struct arg_struct *the_inp = (struct arg_struct *)arguments;
    //std::cout << the_inp->inp << std::endl;
    int cur_window=0;
    int seq_counter=0;
    int oldRemain=0;

    std::ifstream myReadFile;
    myReadFile.open(the_inp->inp);

    std::string line;
    /* Data size */
    int packRemanining=500;

    while(!myReadFile.eof())
    {
        std::getline (myReadFile,line);
        /* To not miss end of line character and get exactly the same output.txt */

        if (myReadFile.good())
        {
            line.push_back('\n');
        }
        else
        {
            continue;
        }
        
        //std::cout << line<<std::endl;
        int inpBase=0;
        int inpRemaining=line.size();
        while(inpRemaining>0)
        {
            pthread_mutex_lock (&mutex);
            /* window size */
            if(READ_WINDOW<10)
            {
                if(inpRemaining>=packRemanining)
                {
                    the_window[cur_window].type=1;
                    the_window[cur_window].seqnum=seq_counter;
                    the_window[cur_window].length=the_window[cur_window].length+packRemanining;
                    //std::cout << "Old len:" << the_window[cur_window].length << " Remain: " << packRemanining << std::endl;
                    the_window[cur_window].data.append(line,inpBase,packRemanining);

                    inpBase=inpBase+packRemanining;
                    inpRemaining=inpRemaining-packRemanining;

                    //std::cout <<" READ_SEQ: " << the_window[cur_window].seqnum << std::endl;
                    
                    cur_window++;
                    /* window size */
                    cur_window=cur_window%10;
                    seq_counter++;
                    seq_counter=seq_counter%32;
                    /* Data size */
                    packRemanining=500;
                    READ_WINDOW++;                    
                }
                else
                {
                    the_window[cur_window].type=1;
                    the_window[cur_window].seqnum=seq_counter;
                    the_window[cur_window].length=the_window[cur_window].length+inpRemaining;
                    the_window[cur_window].data.append(line,inpBase,inpRemaining);

                    inpBase=inpBase+inpRemaining;
                    packRemanining=packRemanining-inpRemaining;
                    inpRemaining=inpRemaining-inpRemaining;
                       
                }    
            }
            pthread_mutex_unlock (&mutex);
            //std::cout << inpRemaining << " "<< inpBase << " " << packRemanining << std::endl;
        }
    }
    /* The last packet before FIN packet if sth left*/
    if(the_window[cur_window].data.length() != 0)
    {
        cur_window++;
        /* window size */
        cur_window=cur_window%10;
        seq_counter++;
        seq_counter=seq_counter%32;
        /* Data size */
        packRemanining=500;
        READ_WINDOW++;   
    }
    while(1)
    {
        /*EOT packet everything is read, sent, ACKed*/
        if( (READ_WINDOW==0) && (SENT==0) )
        {
            pthread_mutex_lock (&mutex);
            the_window[cur_window].type=2;
            the_window[cur_window].seqnum=seq_counter;
            the_window[cur_window].length=0;
            READ_WINDOW++;    
            
            pthread_mutex_unlock (&mutex);
            //std::cout<< "AMN SKIMMMMMMMMMMMMMMMM" << std::endl;
            break;
        }
        else
        {
            usleep(10);
        }

    }
    FINISH_FLAG_1=1;
    std::cout << "Read thread is out" << std::endl;
}
/* This threads sends the packets in the window */
void *sendData(void *arguments)
{
    std::cout << "Sending thread started " << std::endl;

    struct arg_emul *the_inp = (struct arg_emul *)arguments;

    pthread_getcpuclockid(pthread_self(), &timerVar.threadID);

    struct timespec start;
    float time_begin;

    std::ofstream sendfile;
    sendfile.open("seqnum.log");

    /* Packet object size*/
    /* Data size */
    char buff[512];
    int sockfd;
    struct sockaddr_in sendaddr;
    /* UDP */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    {
        perror("Error:Opening socket");
        exit(1);
    }
    bzero((char *) &sendaddr, sizeof(sendaddr));
    /* IPv4 */
    sendaddr.sin_family = AF_INET;

    /* Array to char array conversion */
    char * charArray = new char [the_inp->emulIP.length()+1];
    strcpy (charArray, the_inp->emulIP.c_str());

    sendaddr.sin_addr.s_addr = inet_addr(charArray);
    sendaddr.sin_port = htons(stoi(the_inp->emulPort));

    if (connect(sockfd, (struct sockaddr *)&sendaddr, sizeof(sendaddr)) != 0)
    { 
        printf("Connection with the server failed\n"); 
        exit(0); 
    }
    while(1)
    {
        pthread_mutex_lock (&mutex);
        if( (READ_WINDOW-SENT) > 0 )
        {
            /* packet object size*/
            /* Data size */
            bzero(buff,512);
            /* int to byte conversions */
            buff[0] = (the_window[SENDER_WINDOW_INDX].type >>24);
            buff[1] = (the_window[SENDER_WINDOW_INDX].type>>16);
            buff[2] = (the_window[SENDER_WINDOW_INDX].type>>8);
            buff[3] = (the_window[SENDER_WINDOW_INDX].type);

            buff[4] = the_window[SENDER_WINDOW_INDX].seqnum>>24;
            buff[5] = the_window[SENDER_WINDOW_INDX].seqnum>>16;
            buff[6] = the_window[SENDER_WINDOW_INDX].seqnum>>8;
            buff[7] = the_window[SENDER_WINDOW_INDX].seqnum;

            buff[8] = the_window[SENDER_WINDOW_INDX].length >>24;
            buff[9] = the_window[SENDER_WINDOW_INDX].length>>16;
            buff[10] = the_window[SENDER_WINDOW_INDX].length>>8;
            buff[11] = the_window[SENDER_WINDOW_INDX].length;

            std::copy(the_window[SENDER_WINDOW_INDX].data.begin(), the_window[SENDER_WINDOW_INDX].data.end(), buff+12);
            
            //std::cout <<" SEND_SEQ: " << the_window[SENDER_WINDOW_INDX].seqnum << std::endl;
            
            sendto(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)NULL, sizeof(sendaddr));

            sendfile << the_window[SENDER_WINDOW_INDX].seqnum << '\n';

            /* If EOT is sent */
            if(the_window[SENDER_WINDOW_INDX].type == 2)
            {
                FINISH_FLAG_2=1;
                pthread_mutex_unlock (&mutex);
                break;
            }

            timerVar.lastIndex=SENDER_WINDOW_INDX;

            /* Initialization or timeout occurance */
            if(timerVar.flag == 0)
            {
                

                clock_gettime(timerVar.threadID, &start);
                time_begin = start.tv_sec*1000;
                time_begin = time_begin + (start.tv_nsec / 1000000);
                timerVar.timeBegin=time_begin;

                //std::cout<< "READ:Time is: " << time_begin << std::endl;

                timerVar.flag =1;
                timerVar.initalIndex=SENDER_WINDOW_INDX;
                timerVar.seqNo=the_window[SENDER_WINDOW_INDX].seqnum;
                //std::cout << "start time is :" << time_begin << std::endl;
                /*CHECK THE timerVar IN THE MAIN MAYBE ANOTHER MUTEX*/
            }

            SENDER_WINDOW_INDX++;
            /* Window size*/
            SENDER_WINDOW_INDX=SENDER_WINDOW_INDX%10;
            
            SENT++;

            pthread_mutex_unlock (&mutex);
        }
        else
        {
            //usleep(1);
            pthread_mutex_unlock (&mutex);
        }
    }
    sendfile.close();
    std::cout << "Sender thread is out" << std::endl;
}

void *recACK(void *arguments)
{
    std::cout << "Listening thread started" << std::endl;

    std::ofstream ackfile;
    ackfile.open("ack.log");

    struct arg_rec *the_inp = (struct arg_rec *)arguments;

    
    int sockRec;
    struct sockaddr_in cliaddr; 

    sockRec = socket(AF_INET, SOCK_DGRAM, 0);

    bzero((char *) &cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = INADDR_ANY;
    cliaddr.sin_port = htons(stoi(the_inp->recPort));    

    if((bind(sockRec, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0))
    {
        std::cerr << "The sender listening UDP port is not available \n";
    }

    /* packet object size*/
    /* Data size */
    unsigned char buff_rec[512];

    struct sockaddr_in clientaddr;
    int len = sizeof(clientaddr);

    int initIndexDummy;
    int difference=0;
    int dummy_holder=0;

    
    packet dummy;

    //int COUNTER_DELETE=0;
    
    while(1)
    {
        struct timespec start;
        float time_begin;

        /* packet object size*/
        /* Data size */
        bzero(buff_rec,512);
        recvfrom(sockRec,buff_rec, sizeof(buff_rec),0,(struct sockaddr *)&clientaddr,(socklen_t*)&len);

        //COUNTER_DELETE++;

        /* Char array to int */
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

        

        if ( (dummy.type == 0) || (dummy.type == 2) )
        {
            pthread_mutex_lock (&mutex);

            if(dummy.type == 0)
                ackfile << dummy.seqnum << '\n';

            if( (SEQ_ACKED <= dummy.seqnum) && ( (dummy.seqnum-SEQ_ACKED)<10 ) )
            {
                //std::cout << "packed with seq no: " << dummy.seqnum << " OK " << std::endl;

                difference = (dummy.seqnum - SEQ_ACKED)+1;
                

                /* Move the window */
                SEQ_ACKED=dummy.seqnum+1;
                SEQ_ACKED=SEQ_ACKED%32;

                READ_WINDOW=READ_WINDOW-difference;
                SENT=SENT-difference;

                /* Shift the base in the window */

                initIndexDummy=timerVar.initalIndex;
                for(; difference>0; difference--)
                {
                    the_window[initIndexDummy].reset();
                    initIndexDummy++;
                    initIndexDummy=initIndexDummy%10;
                }
                timerVar.initalIndex=initIndexDummy;


                clock_gettime(timerVar.threadID, &start);

                time_begin = (start.tv_sec*1000);
                time_begin = time_begin + (start.tv_nsec / 1000000);
                timerVar.timeBegin=time_begin;

                //std::cout<< "Time is: " << time_begin << std::endl;
                
                if( FINISH_FLAG_1==1 )
                {
                    pthread_mutex_unlock (&mutex);
                    break;
                }   
            }
            else if ( (SEQ_ACKED > 22) && (dummy.seqnum < 9) )
            {
                dummy_holder = dummy.seqnum+32;

                difference = ( (dummy.seqnum+32) - SEQ_ACKED)+1;
                
                /* Move the window */
                SEQ_ACKED=dummy.seqnum+1;
                SEQ_ACKED=SEQ_ACKED%32;

                READ_WINDOW=READ_WINDOW-difference;
                SENT=SENT-difference;

                /* Shift the base in the window */

                initIndexDummy=timerVar.initalIndex;
                for(; difference>0; difference--)
                {
                    the_window[initIndexDummy].reset();
                    initIndexDummy++;
                    initIndexDummy=initIndexDummy%10;
                    timerVar.initalIndex=initIndexDummy;
                }
                timerVar.initalIndex=initIndexDummy;

                clock_gettime(timerVar.threadID, &start);

                time_begin = (start.tv_sec*1000);
                time_begin = time_begin + (start.tv_nsec / 1000000);
                timerVar.timeBegin=time_begin;

                //std::cout<< "Time is: " << time_begin << std::endl;

                if( FINISH_FLAG_1==1 )
                {
                    pthread_mutex_unlock (&mutex);
                    break;
                }   
            }
            pthread_mutex_unlock (&mutex);
        }
    }
    //std::cout<< "Total received: "<<COUNTER_DELETE<<std::endl;
    ackfile.close();
    std::cout << "ACK thread is out" << std::endl;

}

void *timeOut(void *arguments)
{
    std::cout << "Timer thread started" << std::endl;
    struct timespec start;
    float time_begin;
    std::clock_t curr;

    while(FINISH_FLAG_2==0)
    {
        clock_gettime(timerVar.threadID, &start);
        time_begin = start.tv_sec*1000;
        time_begin = time_begin + (start.tv_nsec / 1000000);

        //std::cout<< "Time is: " << time_begin << std::endl;

        //std::cout << "CLOCK IS:: " << curr-c_start << std::endl;
        /* If timeout occurs */
        if( ( (timerVar.timeBegin + 100) < time_begin) && (FINISH_FLAG_2==0) )
        {
            
            std::cout << "Initial is: " << timerVar.timeBegin << " Current is: " << time_begin << std::endl;
            //std::cout << "Timeout occured" << std::endl;
            SENDER_WINDOW_INDX=timerVar.initalIndex;
            pthread_mutex_lock (&mutex);
            if(timerVar.lastIndex >= timerVar.initalIndex)
            {
                SENT = SENT - ( timerVar.lastIndex - timerVar.initalIndex +1 );
            }
            else
            {
            /* Window size: +5 is the window size +1 since the first index is 0 */
                SENT = SENT - ( timerVar.lastIndex - timerVar.initalIndex + 11 );
            }
            timerVar.flag=0;
            timerVar.timeBegin=time_begin;
            pthread_mutex_unlock (&mutex);
        }
    }
    std::cout << "Timer thread is out" << std::endl;
}

int main(int argc, char** argv)
{

    timerVar.flag=0;

    int win_count_w=0;
    int win_count_r=0;

    pthread_t threadReadText;
    pthread_t threadSend;
    pthread_t threadRec;
    pthread_t threadTimer;

    struct arg_struct argsRead;
    struct arg_emul argsEmul;
    struct arg_rec argsRec;

    argsEmul.emulIP=argv[1];
    argsEmul.emulPort=argv[2];
    argsRec.recPort=argv[3];
    argsRead.inp=argv[4];
    //std::cout << argv[1] << std::endl;
    pthread_create(&threadReadText, NULL, &readText, (void *)&argsRead);
    pthread_create(&threadSend, NULL, &sendData, (void *)&argsEmul);
    pthread_create(&threadRec, NULL, &recACK, (void *)&argsRec);

    while(timerVar.flag == 0)
    {
        usleep(5);
    }
    pthread_create(&threadTimer, NULL, &timeOut, NULL);

    pthread_join (threadReadText,NULL);

    pthread_join (threadRec,NULL);

    pthread_join (threadSend,NULL);
    pthread_join (threadTimer,NULL);

    std::cout << "All is good sender is closing" << std::endl;


}