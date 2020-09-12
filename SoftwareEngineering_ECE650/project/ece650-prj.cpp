// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"

// defined std::cout
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
#include <fstream>

std::ofstream ofs_time;
std::ofstream ofs_size;


class Graph
{
    public:
        Graph(int,int,std::vector<unsigned>*,std::vector<unsigned>*,std::vector<unsigned>,std::vector<unsigned>);
        int V;
        int E;
        std::vector<unsigned>* G;
        std::vector<unsigned>* G2;
        std::vector<unsigned> matrix;
        std::vector<unsigned> matrix2;
        void setV(int);
        void setE(std::string);
        void path(int,int);
};

Graph::Graph(int vertices=0, int edges=0, std::vector<unsigned>* grap=NULL,std::vector<unsigned>* grap2=NULL, std::vector<unsigned> mtx={1}, std::vector<unsigned> mtx2={1})
{
    V = vertices;
    E = edges;
    G = grap;
    G2 = grap2;
    matrix=mtx;
    matrix2=mtx2;
}

void Graph::setV(int ver)
{
    V = ver;

    delete[] G;
    G = new std::vector<unsigned>[V];

    delete[] G2;
    G2 = new std::vector<unsigned>[V];
}

void Graph::setE(std::string inp)
{
    E=0;
    delete[] G;
    G = new std::vector<unsigned>[V];

    delete[] G2;
    G2 = new std::vector<unsigned>[V];

    std::string inter1;
    std::string inter2;
    int state=0;
    int indx=0;
    char temp=inp[indx];
    while(temp != '\0')
    {
        if (state==1)
            inter1.append(inp,indx,1);
        else if (state == 2)
        {
            if (temp == '<')
                state=0;
            else
                inter2.append(inp,indx,1);
        }
        if (temp=='<')
            state=1;
        else if (temp == ',')
            state=2;
        else if (temp == '>')
        {
            state=0;
            unsigned first = (unsigned)std::stoi(inter1);
            unsigned second = (unsigned)std::stoi(inter2);
            if ( (first<V) && (second<V) )
            {
                if (first == second)
                {
                    indx++;
                    temp=inp[indx];
                    inter1.clear();
                    inter2.clear();
                    continue;
                }
                else
                {
                G[first].push_back(second);
                G[second].push_back(first);

                G2[first].push_back(second);
                G2[second].push_back(first);
                }
            }
            else
            {
                std::cerr << "Error: Given edge value is invalid" << std::endl;
                //std::cerr << first << " " << second << std::endl;
                E=-1;
                delete[] G;
                G = new std::vector<unsigned>[V];

                delete[] G2;
                G2 = new std::vector<unsigned>[V];

                break;
            }
            inter1.clear();
            inter2.clear();
        }
    indx++;
    temp=inp[indx];
    }

    /*All edges will be saved in a nested list, in an increasing order*/
    for(int i=0;i<V;i++)
    {
        std::sort( G[i].begin(), G[i].end() );
        G[i].erase( std::unique( G[i].begin(), G[i].end() ), G[i].end() );

        std::sort( G2[i].begin(), G2[i].end() );
        G2[i].erase( std::unique( G2[i].begin(), G2[i].end() ), G2[i].end() );

    }

    //Test through coding to make sure edges are appended correctly
    /*
    for(int o=0;o<V;o++)
    {
        for(auto p=G[o].begin(); p!= G[o].end();++p)
        {
            std::cout << *p << " ";
        }
        std::cout << std::endl;
    }
    */    

    /*In the coming for loop, the edges are appended into a single list to run the code faster and avoid duplications due to symmetry.
    Matrix is actually a list.*/

    matrix.clear();
    matrix2.clear();
    for(int i=0;i<V;i++)
    {
        for (auto k = G[i].begin(); k != G[i].end(); ++k)
        {
            if(i < *k)
            {
                matrix.push_back(i);
                matrix.push_back(*k);

                matrix2.push_back(i);
                matrix2.push_back(*k);

                E++;
            }
        }
    }

    if (E == 0)
    {
        std::cout<< std::endl;
    }
    //Test through coding to make sure matrix works correctly
    /*
    for(auto p=matrix.begin(); p!= matrix.end();++p)
    {
        std::cout << *p << " ";
    }
    std::cout << std::endl;
    */
}

struct arg_struct {
    Graph arg1;
    std::string arg2;
    struct timespec arg3;
    int arg4;
};

void *VC_one(void *arguments)
{
    clockid_t cid;
    pthread_getcpuclockid(pthread_self(), &cid);
    struct timespec start,stop,accum;
    clock_gettime(cid, &start);

    struct arg_struct *args = (struct arg_struct *)arguments;
    Graph map = args -> arg1;
    int curIndx,sz,szTemp;
    int counter;
    std::vector<int> result_vec;

    for(int x=0;x<map.V;x++)
    {
        sz=0;
        counter=0;
        for(int y=0;y<map.V;y++)
        {
            szTemp=map.G2[y].size();
            if(szTemp>sz)
            {
                sz=szTemp;
                curIndx=y;
            }
            if(szTemp==0)
            {
                counter++;
            }
        }
        if(counter==map.V)
        {
            break;
        }
        else
        {            
            result_vec.push_back(curIndx);
            for(auto j=map.G2[curIndx].begin(); j!= map.G2[curIndx].end(); j++)
            {
                map.G2[*j].erase(std::remove(map.G2[*j].begin(), map.G2[*j].end(), curIndx), map.G2[*j].end());
            }
            map.G2[curIndx].clear();   
        }
    }

    std::sort(result_vec.begin(), result_vec.end());
    std::string output;
    args -> arg4 = result_vec.size();
    output ="APPROX-VC-1: ";
    output=output+std::to_string(result_vec[0]);

    for(int k=1; k<result_vec.size(); k++)
        output=output+","+std::to_string(result_vec[k]);
    args -> arg2 = output;

    clock_gettime(cid, &stop);

    accum.tv_sec = ( stop.tv_sec - start.tv_sec );
    accum.tv_nsec = ( stop.tv_nsec - start.tv_nsec );

    args -> arg3 = accum;
    return NULL;
}


//std::vector<int> VC_two(Graph map)
void *VC_two(void *arguments)
{
    clockid_t cid;
    pthread_getcpuclockid(pthread_self(), &cid);
    struct timespec start,stop,accum;
    clock_gettime(cid, &start);

    struct arg_struct *args = (struct arg_struct *)arguments;
    Graph map = args -> arg1;
    std::vector<int> rslt;
    std::ifstream urandom("/dev/urandom");
    if (urandom.fail()) 
    {
        std::cerr << "Error: cannot open /dev/urandom\n";
        return NULL;
    }

    int edgeNo;

    while(map.matrix2.size()>0)
    {
        edgeNo=map.matrix2.size();
        edgeNo=edgeNo/2;


        unsigned int r = 42;
        urandom.read((char*)&r, sizeof(int));
        int ranIdx = ( r % (edgeNo) );

        if(ranIdx%2==1)
        {
            ranIdx=ranIdx-1;
        }
        int vU=map.matrix2[ranIdx];
        int vV=map.matrix2[ranIdx+1];

        rslt.push_back(vU);
        rslt.push_back(vV);
        
        int cntr=0;
        auto head=map.matrix2.begin();
        while(cntr<map.matrix2.size())
        {
            if((*(head+cntr)==vU) || (*(head+cntr+1)==vU))
            {
                map.matrix2.erase(head+cntr+1);
                map.matrix2.erase(head+cntr);
            }
            else
            {
                cntr=cntr+2;
            }
        }
        cntr=0;
        head=map.matrix2.begin();
        while(cntr<map.matrix2.size())
        {
            if((*(head+cntr)==vV) || (*(head+cntr+1)==vV))
            {
                map.matrix2.erase(head+cntr+1);
                map.matrix2.erase(head+cntr);
            }
            else
            {
                cntr=cntr+2;
            }
        }
    }
    std::sort(rslt.begin(), rslt.end());
    std::string output;
    args -> arg4 = rslt.size();
    output ="APPROX-VC-2: ";
    output=output+std::to_string(rslt[0]);
    for(int k=1; k<rslt.size(); k++)
        output=output+","+std::to_string(rslt[k]);
    args -> arg2 = output;


    clock_gettime(cid, &stop);
    //std::cout << "Stop: " <<  stop.tv_sec << ","<< stop.tv_nsec << std::endl;
    //printf("Stop: %4ld.%03ld\n", stop.tv_sec, stop.tv_nsec / 1000000);
    accum.tv_sec = ( stop.tv_sec - start.tv_sec );
    accum.tv_nsec = ( stop.tv_nsec - start.tv_nsec );

    args -> arg3 = accum;
    return NULL;
}

struct arg_struct_SAT {
    std::vector<unsigned> arg1;
    std::string arg2;
    struct timespec arg3;
    int V_no;
    struct timespec start_time;
    int start_flag;
    int terminate_flag;
};

struct arg_struct_SAT argsSAT;

std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
std::vector<Minisat::Lit> varList;
std::string global_output;
int global_variableNo;
Minisat::vec< Minisat::Lit > claus;


std::string clausBuilder(std::vector<unsigned> edgesSs,int n,int k)
{
    global_variableNo=k*n;

    for(int i=0;i<global_variableNo;i++)
    {
        varList.push_back(Minisat::mkLit(solver->newVar()));
    }

    int counterClause=0;

    /*Clauses of the first constraint in the given reduction pdf*/
    for(int cntr=0; cntr<k; cntr++)
    {
        claus.clear();

        for(int dummyK=0; dummyK<global_variableNo; dummyK=dummyK+k)
        {
            claus.push( varList[dummyK+cntr] );
            //std::cout << dummyK+cntr+1 << ",";
        }
        solver->addClause(claus);
        //std::cout << std::endl;
        counterClause++;
    }

    /*Clauses of the second constraint in the given reduction pdf*/
    for(int vert=0;vert<n;vert++)
    {
        int vertIndx=vert*k;
        for(int cntr1=0; cntr1<k-1; cntr1++)
        {
            for(int cntr2=cntr1+1; cntr2<=k-1; cntr2++)
            {
                solver->addClause(~varList[vertIndx+cntr1],~varList[vertIndx+cntr2]);
                //std::cout << "~"<< vertIndx+cntr1+1 << "," << "~"<< vertIndx+cntr2+1 << std::endl;
                counterClause++;
            }
        }
    }
    /*Clauses of the third constraint in the given reduction pdf*/
    for(int var=0; var<k; var++)
    {
        for(int cntr1=0;cntr1<n-1; cntr1++)
        {
            for(int cntr2=cntr1+1; cntr2<=n-1; cntr2++)
            {
                solver->addClause(~varList[var+(cntr1*k)],~varList[var+(cntr2*k)]);
                //std::cout << "~"<< var+(cntr1*k)+1 << "," << "~"<< var+(cntr2*k)+1 << std::endl;
                counterClause++;
            }
        }
    }
    /*Clauses of the fourth constraint in the given reduction pdf*/
    for (int cntr = 0; cntr <  edgesSs.size(); cntr=cntr+2)
    {
        int i=edgesSs[cntr];
        int j=edgesSs[cntr+1];

        claus.clear();

        for(int cntr1=0; cntr1 < k; cntr1++)
        {
            claus.push(varList[(j*k)+cntr1]);
            claus.push(varList[(i*k)+cntr1]);
            //std::cout << (j*k)+cntr1+1 << "," << (i*k)+cntr1+1<< ",";
        }

        solver->addClause(claus);
        //std::cout << std::endl;
        counterClause++;
    }

    //std::cout << counterClause << std::endl;
    
    //std::cerr<<"In \n";
    bool res = solver->solve();
    //std::cerr<<"Out \n";
    
    /*If there is an answer, in this part we get the vertices for the cover, otherwise we return an empty vector*/
    if (res==true)
    {
        std::vector<int> rslt;
        for (int i=0;i<global_variableNo;i++)
        {
            if(Minisat::toInt(solver->modelValue(varList[i]))==0)
            {
                rslt.push_back(i);
            }
            else
            {
                continue;
            }
        }
        int tempNumber;

        std::sort(rslt.begin(), rslt.end());

        global_output ="CNF-SAT-VC: ";
        tempNumber = rslt[0]/k;

        global_output=global_output+std::to_string(tempNumber);

        for(int indx=1; indx<rslt.size(); indx++)
        {
            tempNumber = rslt[indx]/k;
            global_output=global_output+","+std::to_string(tempNumber);
        }
        return global_output;
    }
    else
    {
        return global_output;
    }
}

 void *VC_SAT(void *unused)
{
    pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &(argsSAT.arg3));
    argsSAT.start_flag=1;

    for(int kVal=1;kVal<=argsSAT.V_no;kVal++)
    {
        usleep(100000);
        solver.reset(new Minisat::Solver());
        varList.clear();
        global_output.clear();
        global_output=clausBuilder(argsSAT.arg1,argsSAT.V_no,kVal);
        if (global_output.size()>0)
        {
            break;
        }
        else
        {
            continue;
        }
        
    }
    argsSAT.arg2 = global_output;

    argsSAT.terminate_flag=1;

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &(argsSAT.arg3));

    return NULL;
}


static Graph haritoDes;

int main(int argc, char **argv) 
{
    int time_out_margin=30;
    std::string line;
    while(!std::cin.eof())
    {
        std::getline (std::cin,line);

        if (line[0]=='V')
        {
            std::string number;
            number.append(line.begin()+1,line.end());
            int no;
            no=std::stoi(number);
            if (no < 1)
            {
                std::cerr<<"Error: There has to be more than 1 vertex";
                std::cout<<std::endl;
            }
            else
            {
                haritoDes.setV(no);
            }
        }
        else if(line[0]=='E')
        {
            std::string edges;
            edges.append(line.begin()+1,line.end());
            haritoDes.setE(edges);
            
            /*This is the case if there is no edge or invalid edge, then all the vertices must be printed out"*/
            if (haritoDes.E < 1)
            {
                continue;
            }
            /*If there is at least an edge*/
            else
            {
                std::string outputs;
                outputs.clear();
                struct timespec timeTh;
                struct timespec timeOutStart;
                struct timespec timeOutCurrent;
                std::vector<unsigned> edgesSAT;

                for (int ii = 0; ii <  haritoDes.matrix.size(); ii++)
                {
                    edgesSAT.push_back(haritoDes.matrix[ii]);
                }

                pthread_t threadSAT;
                pthread_t threadVC1;
                pthread_t threadVC2;

                argsSAT.arg1=edgesSAT;
                argsSAT.V_no=haritoDes.V;
                argsSAT.arg2=outputs;
                argsSAT.arg3=timeTh;
                argsSAT.start_time=timeOutStart;
                argsSAT.start_flag=0;
                argsSAT.terminate_flag=0;

                
                struct arg_struct argsVC1;
                argsVC1.arg1=haritoDes;
                argsVC1.arg2=outputs;
                argsVC1.arg3=timeTh;
                argsVC1.arg4=0;

                struct arg_struct argsVC2;
                argsVC2.arg1=haritoDes;
                argsVC2.arg2=outputs;
                argsVC2.arg3=timeTh;
                argsVC2.arg4=0;

                pthread_create(&threadSAT, NULL, &VC_SAT, NULL);
                pthread_create(&threadVC1, NULL, &VC_one,(void *)&argsVC1);
                pthread_create(&threadVC2, NULL, &VC_two,(void *)&argsVC2);

                while(argsSAT.start_flag==0)
                {
                    usleep(10000);
                }

                clockid_t SAT_thread_ID;
                pthread_getcpuclockid(threadSAT, &SAT_thread_ID);
                clock_gettime(SAT_thread_ID, &timeOutCurrent);

                while( ( (argsSAT.arg3.tv_sec + time_out_margin) > timeOutCurrent.tv_sec ) && (argsSAT.terminate_flag==0) )
                {
                    clock_gettime(SAT_thread_ID, &timeOutCurrent);
                    //std::cout << timeOutCurrent.tv_sec << std::endl;
                    usleep(10000);
                }

                //std::cerr<<"StartT "<<timeOutCurrent.tv_sec << "\n";

                //flag_SAT_terminate=1 shows for loop of SAT has been finished, either an answer found or timeout happened
                if(argsSAT.terminate_flag==0)
                {
                    solver->interrupt();

                    usleep(1000);

                    pthread_cancel(threadSAT);

                    usleep(1000);

                    pthread_join (threadSAT,NULL );

                    //std::cerr<<"Error: Timeout happened during SAT solver \n";

                     pthread_join (threadVC1,NULL);
                     pthread_join (threadVC2,NULL);    
                    
                    std::cerr << "CNF-SAT-VC: timeout" << std::endl;
                    std::cout << argsVC1.arg2 << std::endl;
                    std::cout << argsVC2.arg2 << std::endl;              
                }
                else
                {
                     usleep(1000);

                    pthread_join (threadSAT,NULL);

                    pthread_join (threadVC1,NULL);

                    pthread_join (threadVC2,NULL);

                    usleep(1000);

                    std::cout << argsSAT.arg2 << std::endl;
                    std::cout << argsVC1.arg2 << std::endl;
                    std::cout << argsVC2.arg2 << std::endl;

                     usleep(1000);
                }
            }
        } 
    }
    return 0;
}
