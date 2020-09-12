// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>


class Graph
{
    public:
        Graph(int, std::vector<unsigned>*);
        int V;
        std::vector<unsigned>* G;
        void setV(int);
        void setE(std::string);
        void path(int,int);
};

Graph::Graph(int vertices=0, std::vector<unsigned>* grap=NULL)
{
    V = vertices;
    G = grap;
}

void Graph::setV(int ver)
{
    delete[] G;
    V = ver;
    G = new std::vector<unsigned>[V];
}

void Graph::setE(std::string inp)
{
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
                break;
            }
            inter1.clear();
            inter2.clear();
        }
    indx++;
    temp=inp[indx];
    }
    for(int i=0;i<V;i++)
    {
        std::sort( G[i].begin(), G[i].end() );
        G[i].erase( std::unique( G[i].begin(), G[i].end() ), G[i].end() );
    }
}

void Graph::path(int source,int dest)
{
    int* color;
    color = new int[V];
    int* pi;
    pi = new int[V];
    int* d;
    d = new int[V];
    for(int k=0;k<V;k++)
    {
        color[k]=0;
        pi[k]=-1;
        d[k]=V+100;
    }
    std::queue<int> Qlist;
    Qlist.push(source);
    color[source]=0;
    d[source]=0;
    while(!Qlist.empty())
    {
        int invest = Qlist.front();
        Qlist.pop();
        color[invest]=2;
        for ( auto l = G[invest].begin(); l != G[invest].end(); l++ )
        {
            if(color[*l] == 0)
            {
                Qlist.push(*l);
                color[*l]=1;
                d[*l]=d[invest]+1;
                pi[*l]=invest;
            }
            else if(d[*l]>d[invest]+1)
            {
                d[*l]=d[invest]+1;
                pi[*l]=invest;
            }
        }

    }
    if (color[dest] != 2)
    {
        std::cerr << "Error: There is no path from given source to destination" << std::endl;
    }
    else
    {
        std::stack<int> path;
        int head=dest;
        path.push(head);
        while(head != source)
        {
            head=pi[head];
            path.push(head);
        }
        int prin;
        prin = path.top();
        path.pop();
        std::cout << prin;
        while(!path.empty())
        {
            prin = path.top();
            path.pop();
            std::cout << "-" << prin;
        }
        std::cout << std::endl;
    }
    delete[] color;
    delete[] pi;
    delete[] d;
}

int main(int argc, char** argv) {
    Graph haritoDes;
    std::string line;
    std::getline (std::cin,line);
    while(!std::cin.eof())
    {
        if (line[0]=='V')
        {
            std::cout << line << std::endl;
            std::string number;
            number.append(line.begin()+1,line.end());
            int no;
            no=std::stoi(number);
            if (no < 0)
            {
                std::cerr<<"There has to be more than 1 vertices";
                std::cout<<std::endl;
            }
            else
            {
                haritoDes.setV(no);
            }
        }
        else if(line[0]=='E')
        {
            std::cout << line << std::endl;
            std::string edges;
            edges.append(line.begin()+1,line.end());
            haritoDes.setE(edges);
        }
        else if(line[0]=='s')
        {
            if (haritoDes.V > 0)
            {
                int controll=0;
                int cnt=0;
                int sour,des;
                std::string s,d;
                for (auto v = line.begin()+1; v != line.end(); v++)
                {
                    cnt++;
                    if (*v != ' ' && controll==0)
                    {
                        controll=1;
                    }
                    else if(*v == ' ' && controll==1)
                    {
                        controll=2;
                    }
                    else if(*v != ' ' && controll==2)
                    {
                        controll=3;
                    }
                    if(controll==1)
                    {
                        s.append(line,cnt,1);
                    }
                    else if(controll == 3)
                    {
                        d.append(line,cnt,1);
                    }
                }
                sour=std::stoi(s);
                des=std::stoi(d);

                if ( ( sour<(haritoDes.V) ) && ( des<(haritoDes.V) ) )
                {
                    if (s == d)
                    {
                        std::cout << sour << "-" << des << std::endl;
                    }
                    else
                    {
                    haritoDes.path(sour,des);
                    }
                }
                else
                {
                    std::cerr << "Error: The source or the destination is not valid" << std::endl;
                }
            }
            else
            {
                continue;
            }
        }
    std::getline (std::cin,line);
    }
    return 0;
}

