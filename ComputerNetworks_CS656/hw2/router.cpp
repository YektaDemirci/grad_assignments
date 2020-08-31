#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <math.h>
#include <set>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector> 

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>


#define NBR_ROUTER 5


struct router_cost
{
    unsigned int router;
    unsigned int cost;
};

struct link_cost
{
    unsigned int link;
    unsigned int cost;
};

struct link_var
{
    unsigned int lin;
    unsigned int cos;
    unsigned int r1;
    unsigned int r2;
};

struct rlc
{
    unsigned int ro;
    unsigned int li;
    unsigned int co;
};

struct sent_packet
{
    unsigned int sent_rout;
    unsigned int sent_link;
    unsigned int sent_cost;
    unsigned int sent_path;
};

struct l_ctr
{
    unsigned int linkNo;
    unsigned int cntr;
    unsigned int r1;
    unsigned int r2;
};

/* Char array to int, function */
int char_int(char * ptr)
{
    int int_version=0;
    int ctr=0;
    int temp=0;
    while(ptr[ctr] != '\0')
        ctr++;
    int digit=(ctr-1);
    for(int i=0; i<ctr;i++)
    {
        temp = (int) ptr[digit];
        int_version= ( (temp-48)*pow (10,i) ) + int_version;  
        digit--; 
    }
    return int_version;
}

/* Byte array to int converter */
int byte_int(unsigned char * ptr)
{
    int temp=0;
    temp = temp+(int)(ptr[0] );
    temp = temp+(int)(ptr[1] << 8);
    temp = temp+(int)(ptr[2] << 16);
    temp = temp+(int)(ptr[3] << 24);
    return temp;
}

/* Int to byte array converter */
void int_byte(unsigned char * ptr, int val)
{
    *(ptr+3) = (val >>24);
    *(ptr+2) = (val>>16);
    *(ptr+1) = (val>>8);
    *(ptr+0) = (val);
}

void dijkstra(std::vector<router_cost> * graph, int source, int * dis, int * prev)
{
    int tempDist;
    int u;
    std::set<int> Q;
    std::set<int>::iterator it;

    Q.clear();
    for(int i=0;i<NBR_ROUTER;i++)
    {
        dis[i]=999999;
        prev[i]=-999999;
        Q.insert(i);
    }
    dis[source]=0;
    while(!Q.empty())
    {
        tempDist=9999999;
        for (it=Q.begin(); it!=Q.end(); ++it)
        {
            if(dis[*it]<tempDist)
            {
                tempDist=dis[*it];
                u=*it;
            }
        }
        Q.erase(u);
        for(int i=0; i<graph[u].size(); i++)
        {
            tempDist = dis[u] + graph[u][i].cost;
            if(tempDist < dis[(graph[u][i].router)-1])
            {
                dis[(graph[u][i].router)-1]=tempDist;
                prev[(graph[u][i].router)-1]=u+1;
            }
        }
    }
}
int main(int argc, char** argv)
{
    /* Initialization of a log file */
    std::ofstream logfile;
    char logName[] ="routerX.log";
    /* Updating log file ID */
    logName[6]=*argv[1];
    //std::cout << logName << std::endl;
    logfile.open(logName);
    /* Unique overall linkData, link var is <link_id,cost,router_1,router_2>, to check if the link is already saved or if there is new router */
    std::vector<link_var> linkData;
    /* Unique overall routerData, same information as linkData, different structure for easy Dijsktra implementation */
    std::vector<router_cost> routerData[NBR_ROUTER];
    /* For easy logging */
    std::vector<link_cost> linkLog[5];
    /* All links */
    std::vector<rlc> allLinks;
    /* Link counter to see if RIB changes */
    std::vector<l_ctr> links_cntr;
    /* Neighbour Link # */
    int NL[NBR_ROUTER];
    /* RIB data */
    router_cost RIB[NBR_ROUTER];
    /* Neighbours to flood the received packets */
    std::vector<int> helloRecieved;
    /* Initial pkt_LSPDUs data */
    std::vector<rlc> toSend;
    /* for Dijkstra */
    int dist[NBR_ROUTER];
    int prev[NBR_ROUTER];
    /* To track sent packets and avoid loops */
    std::vector<sent_packet> sent_packets;

    /* Neighbour links unkown yet */
    for(int k=0;k<NBR_ROUTER;k++)
    {
        NL[k]=1959;
    }

    int ID=0;
    for(int i=0; i<5; i++)
    {
        RIB[i].router=99999;
        RIB[i].cost=99999;
    }

    int nse_port=0;
    int router_port=0;
    
    /* Char array to int */
    ID=char_int(argv[1]);
    nse_port=char_int(argv[3]);
    router_port= char_int(argv[4]);

    /* Creating the port */
    int socketrtr;
    struct sockaddr_in routerAddr;

    socketrtr = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketrtr < 0) 
    {
        perror("Error:Opening the socket");
        exit(1);
    }

    bzero((char *) &routerAddr, sizeof(routerAddr));
    routerAddr.sin_family = AF_INET;                            /* IPv4 */
    routerAddr.sin_addr.s_addr = INADDR_ANY;        /* IP of the receiver */ 
    routerAddr.sin_port = htons(router_port);

    if((bind(socketrtr, (struct sockaddr *) &routerAddr, sizeof(routerAddr)) < 0))
    {
        std::cerr << "The router listening UDP port is not available \n";
    }


    struct sockaddr_in nseAddr;
    int len = sizeof(nseAddr);

    bzero((char *) &nseAddr, sizeof(nseAddr));
    nseAddr.sin_family = AF_INET;                    /* IPv4 */
    nseAddr.sin_addr.s_addr = inet_addr(argv[2]);    /* IP of the nse */ 
    nseAddr.sin_port = htons(nse_port);

    unsigned char socketpkt_INIT_byte[4];
    /* creating socketpkt_INIT_byte */
    bzero(socketpkt_INIT_byte,4);

    int_byte(socketpkt_INIT_byte, ID);
    /*
    socketpkt_INIT_byte[3] = (ID >>24);
    socketpkt_INIT_byte[2] = (ID>>16);
    socketpkt_INIT_byte[1] = (ID>>8);
    socketpkt_INIT_byte[0] = (ID);
    */

    unsigned char circuit_DB_byte[44];
    bzero(circuit_DB_byte,44);


    /* Sending socketpkt_INIT_byte */
    sendto(socketrtr, socketpkt_INIT_byte, sizeof(socketpkt_INIT_byte), 0, (struct sockaddr*) &nseAddr, sizeof(nseAddr));
    /* Writing: sending INIT to log */
    logfile << "R"<< ID<< " sends an INIT: router_id " << ID << '\n';


    /* Receiving neighbour information */
    recvfrom(socketrtr, circuit_DB_byte, sizeof(circuit_DB_byte), 0, (struct sockaddr*) &nseAddr, (socklen_t*)&len);

    /* Number of neighbours */
    int neigh;
    neigh=byte_int(circuit_DB_byte);
    /* Writing: # of links log */
    logfile << "R"<< ID<< " receives a CIRCUIT_DB: nbr_link " << neigh << '\n';

    
    link_cost temp;
    rlc tempRLC;
    l_ctr temp_link;
    /*Byte to circ_db object */
    for(int i=0;i<neigh;i++)
    {
        tempRLC.ro=ID;
        tempRLC.li=byte_int(circuit_DB_byte+(4+(8*i)));
        tempRLC.co=byte_int(circuit_DB_byte+(8*(i+1)));
        allLinks.push_back(tempRLC);

        temp_link.linkNo=tempRLC.li;
        temp_link.cntr=1;
        temp_link.r1=ID;
        temp_link.r2=1959;
        links_cntr.push_back(temp_link);

        temp.link=tempRLC.li;
        temp.cost=tempRLC.co;
        linkLog[ID-1].push_back(temp);
    }
    /* First database logging */
    logfile << "# Topology database \n";
    for(int i=0; i<5; i++)
    {
        if(linkLog[i].size())
            logfile << "R"<<ID <<" -> R"<<i+1<< " nbr link "<<linkLog[i].size() << "\n";
        for(int j=0; j<linkLog[i].size();j++)
        {
            logfile << "R"<<ID <<" -> R"<<i+1<< " link "<<linkLog[i][j].link << " cost "<<linkLog[i][j].cost <<"\n";
        }
    }
    logfile << "# RIB \n";
    for(int i=0;i<5;i++)
    {
        if(i==(ID-1))
            logfile << "R"<<ID <<" -> R"<<i+1<< " -> Local, 0 \n";
        else
        {
            if(RIB[i].router!=99999)
                logfile << "R"<<ID <<" -> R"<<i+1<< " -> R"<<RIB[i].router<<", " <<RIB[i].cost << "\n";
            else
                logfile << "R"<<ID <<" -> R"<<i+1<< " -> INF, INF \n";
        }
        
    }

    /* Sending HELLO messages */
    unsigned char pkt_HELLO[8];
    for(int i=0;i<neigh;i++)
    {
        bzero(pkt_HELLO,8);
        /* router ID */
        int_byte(pkt_HELLO,ID);
        /* link ID */
        strncpy ( (char *)(pkt_HELLO+4), (char *) (circuit_DB_byte+(4+(8*i)) ), 4 );
        sendto(socketrtr, pkt_HELLO, sizeof(pkt_HELLO), 0, (struct sockaddr*) &nseAddr, sizeof(nseAddr));
        logfile << "R"<< ID<< " sends a HELLO: router_id " << ID << " link_id "<< allLinks[i].li << '\n';

    }

    /* Receiving HELLO messages */
    int sender=0;
    int linker=0;
    rlc temp2;
    link_var temp3;
    router_cost temp4;

    for(int i=0;i<neigh;i++)
    {
        bzero(pkt_HELLO,8);
        recvfrom(socketrtr, pkt_HELLO, sizeof(pkt_HELLO), 0, (struct sockaddr*) &nseAddr, (socklen_t*)&len);
        //std::cout << "Hello is received from: ";

        sender=byte_int(pkt_HELLO);
        linker=byte_int(pkt_HELLO+4);
        logfile << "R"<< argv[1]<< " receives a HELLO: router_id " << sender<< " link_id "<< linker << '\n';

        helloRecieved.push_back(sender);

        //Link to neighbour
        NL[sender-1]=linker;        
    }

    unsigned char pkt_LSPDU[20];
    sent_packet temp_sent;
    for(int i=0; i < helloRecieved.size();i++)
    {
        /* allLinks is equivalent to circuit_DB at this point */
        for(int j=0; j < allLinks.size();j++)
        {
            bzero(pkt_LSPDU,20);
            int_byte(pkt_LSPDU,ID);
            int_byte((pkt_LSPDU+4),allLinks[j].ro);
            int_byte((pkt_LSPDU+8),allLinks[j].li);
            int_byte((pkt_LSPDU+12),allLinks[j].co);
            int_byte((pkt_LSPDU+16),NL[helloRecieved[i]-1]);
            sendto(socketrtr, pkt_LSPDU, sizeof(pkt_LSPDU), 0, (struct sockaddr*) &nseAddr, sizeof(nseAddr));
            logfile << "R"<< ID<< " sends an LSPDU: sender " << ID << ", router_id "<< allLinks[j].ro << ", link_id " << allLinks[j].li << ", cost "<< allLinks[j].co <<", via "<< NL[helloRecieved[i]-1] <<  '\n';
            /* In order to not send the same packet again, somehow */
        }
    }

    int rec_ID;
    int rec_rout;
    int rec_link;
    int rec_cost;
    int rec_path;
    /* Flag to check if the received link is in the database */
    int flag_1;
    /* Flag to check if the database is changed */
    int flag_2;
    /* If the packet is sent to that router before to avoid loops*/
    int flag_3;

    int linkDataLenght=0;

    int tempPrev;
    
    int sret;
    fd_set readfds;
    struct timeval timeout;

    //std::cout<<"Socket no is: " << socketrtr << std::endl;
    /* Router will listen its port, update its database and propagate the received message with neighbours */
    int ctr=0;
    
    while(1)
    {   
        FD_ZERO(&readfds);
        FD_SET(socketrtr, &readfds);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        sret = select (socketrtr+1, &readfds, NULL, NULL, &timeout);
        //std::cout << sret << std::endl;
        if(sret ==0)
        {
            std::cout << "Timeout" << std::endl;
            break;
        }
        else
        {

            flag_1=0;
            flag_2=1;

            bzero(pkt_LSPDU,20);
            //std::cout<<"rec ing"<<std::endl;
            recvfrom(socketrtr, pkt_LSPDU, sizeof(pkt_LSPDU), 0, (struct sockaddr*) &nseAddr, (socklen_t*)&len);
            //std::cout<<"rec ed" << std::endl;
            //std::cout << "received \n";
            
            rec_ID= byte_int(pkt_LSPDU);
            rec_rout=byte_int(pkt_LSPDU+4);
            rec_link=byte_int(pkt_LSPDU+8);
            rec_cost=byte_int(pkt_LSPDU+12);
            rec_path=byte_int(pkt_LSPDU+16);

            logfile << "R"<< ID<< " receives an LSPDU: sender " << rec_ID << ", router_id "<< rec_rout << ", link_id " << rec_link << ", cost "<< rec_cost <<", via "<< rec_path <<  '\n';


            linkDataLenght=allLinks.size();

            for(int i=0; i<linkDataLenght; i++)
            {
                if ( (allLinks[i].ro==rec_rout) && (allLinks[i].li==rec_link) && (allLinks[i].co==rec_cost) )
                {
                    flag_1=1;
                    break;
                }
            }
            /* It is a new link */
            if(flag_1==0)
            {
                temp.link=rec_link;
                temp.cost=rec_cost;
                linkLog[rec_rout-1].push_back(temp);

                tempRLC.ro=rec_rout;
                tempRLC.li=rec_link;
                tempRLC.co=rec_cost;
                allLinks.push_back(tempRLC);

                for(int i=0; i < links_cntr.size(); ++i)
                {
                    if (links_cntr[i].linkNo == rec_link)
                    {
                        if(links_cntr[i].cntr==1)
                        {
                            links_cntr[i].cntr=links_cntr[i].cntr+1;
                            temp4.cost=rec_cost;
                            temp4.router=links_cntr[i].r1;
                            routerData[rec_rout-1].push_back(temp4);
                            temp4.router=rec_rout;
                            routerData[links_cntr[i].r1-1].push_back(temp4);
                            //std::cerr << "routerData is updated \n";
                        }
                        else
                            std::cerr << "STTTTTH UNEEEEEEEEEEEXPEEEEEECTEEEEEEEED HERE";
                        flag_2=0;
                        break;
                    }

                }
                if( (flag_1==0) && (flag_2==1) )
                {
                    temp_link.linkNo=rec_link;
                    temp_link.cntr=1;
                    temp_link.r1=rec_rout;
                    temp_link.r2=1959;
                    links_cntr.push_back(temp_link);
                }


                dijkstra(routerData, ID-1, dist, prev);

                /* Update RIB */
                for(int i=0;i<5;i++)
                {
                    if(dist[i]!=999999)
                        RIB[i].cost=dist[i];
                    
                    if(prev[i]!=-999999)
                    {
                        if(prev[i]==ID)
                        {
                            RIB[i].router=i+1;
                        }
                        else
                        {
                            //std::cout << "loop ing"<<std::endl;
                            //std::cout << i << std::endl;
                            tempPrev=prev[i];
                            while(1)
                            {
                                if(prev[tempPrev-1]==ID)
                                {
                                    RIB[i].router=tempPrev;
                                    break;
                                }
                                else
                                {
                                tempPrev=prev[tempPrev-1]; 
                                }
                            }
                            //std::cout << "loop ed"<<std::endl;
                        }
                    }
                }/* End of RIB update */
                /* Logging the new database and RIB */
                logfile << "# Topology database \n";
                for(int i=0; i<5; i++)
                {
                    if(linkLog[i].size())
                        logfile << "R"<<ID <<" -> R"<<i+1<< " nbr link "<<linkLog[i].size() << "\n";
                    for(int j=0; j<linkLog[i].size();j++)
                    {
                        logfile << "R"<<ID <<" -> R"<<i+1<< " link "<<linkLog[i][j].link << " cost "<<linkLog[i][j].cost <<"\n";
                    }
                }
                logfile << "# RIB \n";
                for(int i=0;i<5;i++)
                {
                    if(i==(ID-1))
                        logfile << "R"<<ID <<" -> R"<<i+1<< " -> Local, 0 \n";
                    else
                    {
                        if(RIB[i].router!=99999)
                            logfile << "R"<<ID <<" -> R"<<i+1<< " -> R"<<RIB[i].router<<", " <<RIB[i].cost << "\n";
                        else
                            logfile << "R"<<ID <<" -> R"<<i+1<< " -> INF, INF \n";
                    }
                    
                }
                /* Update the sender ID */
                int_byte(pkt_LSPDU,ID);
                /* Flood the received data */
                for(int i=0; i < helloRecieved.size(); ++i)
                {
                    /* To not send to the neighbour where the packet is received from */
                    if(helloRecieved[i]!=rec_ID)
                    {
                        int_byte((pkt_LSPDU+16),NL[helloRecieved[i]-1]);
                        //std::cout<<"send ing"<<std::endl;
                        sendto(socketrtr, pkt_LSPDU, sizeof(pkt_LSPDU), 0, (struct sockaddr*) &nseAddr, sizeof(nseAddr));
                        logfile << "R"<< ID<< " sends an LSPDU: sender " << ID << ", router_id "<< rec_rout << ", link_id " << rec_link << ", cost "<< rec_cost <<", via "<< NL[helloRecieved[i]-1] <<  '\n';
                        //std::cout<<"send ing"<<std::endl;
                    }   
                }
            }/* End of if the database is updated */
        }/* End of timer */
    } /* End of while(1) */
    
 
    
    std::cout << "Final router data is here \n";
    for(int i=0;i<5;i++)
    {
        std::cout<< i+1 << ":";
        for(int j=0;j<routerData[i].size();j++)
            std::cout << routerData[i][j].router << "," << routerData[i][j].cost << ";";
        std::cout<< "\n";
    }

    

    std::cout << "Final RIB \n";
    for(int i=0;i<5;i++)
    {
        std::cout<< i+1 << ":" << RIB[i].router<< "," << RIB[i].cost << std::endl;
    }

    std::cout << "Prev \n";
    for(int i=0;i<5;i++)
    {
        std::cout<< prev[i] << std::endl;
    }

    /*
    std::cout << "NL \n";
    for(int i=0;i<5;i++)
    {
        std::cout<< i+1 << ":" << NL[i] << std::endl;
    }
    */
    
       

}