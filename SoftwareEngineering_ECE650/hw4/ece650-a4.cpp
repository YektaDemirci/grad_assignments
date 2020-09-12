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


class Graph
{
    public:
        Graph(int,int, std::vector<unsigned>*,std::vector<unsigned>);
        int V;
        int E;
        std::vector<unsigned>* G;
        std::vector<unsigned> matrix;
        void setV(int);
        void setE(std::string);
        void path(int,int);
};

Graph::Graph(int vertices=0, int edges=0, std::vector<unsigned>* grap=NULL, std::vector<unsigned> mtx={1})
{
    V = vertices;
    E = edges;
    G = grap;
    matrix=mtx;
}

void Graph::setV(int ver)
{
    delete[] G;
    V = ver;
    G = new std::vector<unsigned>[V];
}

void Graph::setE(std::string inp)
{
    E=0;
    delete[] G;
    G = new std::vector<unsigned>[V];

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
                }
            }
            else
            {
                std::cerr << "Error: Given edge value is invalid" << std::endl;
                //std::cerr << first << " " << second << std::endl;
                E=-1;
                delete[] G;
                G = new std::vector<unsigned>[V];
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
    for(int i=0;i<V;i++)
    {
        for (auto k = G[i].begin(); k != G[i].end(); ++k)
        {
            if(i < *k)
            {
                matrix.push_back(i);
                matrix.push_back(*k);
                E++;
            }
        }
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

std::vector<int> clausBuilder(Graph map,int k)
{
    int n=map.V;
    int variableNo=k*n;

    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());

    std::vector<Minisat::Lit> varList;
    varList.clear();

    for(int i=0;i<variableNo;i++)
    {
        varList.push_back(Minisat::mkLit(solver->newVar()));
    }

    int counterClause=0;

    /*Clauses of the first constraint in the given reduction pdf*/
    for(int cntr=0; cntr<k; cntr++)
    {
        Minisat::vec< Minisat::Lit > claus;
        claus.clear();
        for(int dummyK=0; dummyK<variableNo; dummyK=dummyK+k)
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
    for (int cntr = 0; cntr <  map.matrix.size(); cntr=cntr+2)
    {
        int i=map.matrix[cntr];
        int j=map.matrix[cntr+1];

        Minisat::vec< Minisat::Lit > claus;
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
    
    
    bool res = solver->solve();
    std::vector<int> rslt;
    rslt.clear();
    
    /*If there is an answer, in this part we get the vertices for the cover, otherwise we return an empty vector*/
    if (res)
    {
        for (int i=0;i<variableNo;i++)
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
    }
    return rslt;
}


int main(int argc, char **argv) 
{
    Graph haritoDes;
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
        
            /*dummy will hold the result of minisat solver*/
            std::vector<int> dummy;
            if (haritoDes.E > 0)
            {
                /*We check all the possibilities starting from 1 vertex, kk shows the total vertex number,k*/
                for(int kk=1; kk<=haritoDes.V; kk++)
                {
                    dummy.clear();
                    dummy=clausBuilder(haritoDes,kk);
                    /*If the returned vector is empty it means for the given kk values(which is actually k),
                    there is no vertex cover*/
                    if(dummy.size()>0)
                    {
                        int temporary;
                        /*The division of kk comes from the way I mapped each vertex to each variables.*/
                        temporary = dummy[0]/kk;
                        std::cout<< temporary;
                        /*We should print out the vertices in an increasing order, found_vertices represents the satisfying vertices*/
                        for(int found_vertices=1; found_vertices< dummy.size();found_vertices++)
                        {
                            temporary = dummy[found_vertices]/kk;
                            std::cout <<" " << temporary;
                        }
                        std::cout << std::endl;
                        break;
                    }
                }
            }
            else if(haritoDes.E == -1)
            {
                continue;
            }
            else
            {
                std::cout << std::endl;
            }
            
        } 
    }
    return 0;
}
