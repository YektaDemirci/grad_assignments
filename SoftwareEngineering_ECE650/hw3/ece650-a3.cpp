#include<vector>
#include<unistd.h>
#include<signal.h>
#include<iostream>
#include<string>
#include<sys/wait.h>

class randomInit
{
    public:
        randomInit(std::string, std::string, std::string, std::string);
        std::string s,n,l,c;
};

randomInit::randomInit(std::string S="10", std::string N="5", std::string L="5", std::string C="20")
{
    s=S;
    n=N;
    l=L;
    c=C;
}

int setter(char let, std::string no, randomInit *ini)
{
    if (let == 's')
    {
        if (stoi(no) < 2)
        {
            std::cerr << "Error: s cant be smaller than 2" << std::endl;
            return 1;
        }
        else
        {
            ini->s = no;
        }
    }
    else if (let == 'n')
    {
        if (stoi(no) < 1)
        {
            std::cerr << "Error: n cant be smaller than 1" << std::endl;
            return 1;
        }
        else
        {
        ini->n = no;
        }
    }
    else if (let == 'l')
    {
        if (stoi(no) < 5)
        {
            std::cerr << "Error: l cant be smaller than 5" << std::endl;
            return 1;
        }
        else
        {
            ini->l = no;
        }
    }
    else if (let == 'c')
    {
        if (stoi(no) < 1)
        {
            std::cerr << "Error: c cant be smaller than 1" << std::endl;
            return 1;
        }
        else
        {
            ini->c = no;
        }
    }
    return 0;
}

int main (int argc, char **argv)
{
    randomInit init;
    
    int chck=0;

    for(int argvCounter=1; argvCounter<argc; argvCounter=argvCounter+2)
    {
        std::string letterHolder=argv[argvCounter];
        char s = letterHolder[1];

        std::string number = argv[argvCounter+1];
        chck=setter(s, number, &init);

        if(chck == 1)
        {
            return 1;
        }

    }

    std::vector<pid_t> kids;
    int pipe1[2];

    pipe(pipe1);

    pid_t child_one;
    child_one = fork();

    if (child_one < 0)
    {
        std::cerr << "Error: fork() failed";
        exit(-1);
    }
    else if (child_one == 0)
    {
        int pipe2[2];
        pipe(pipe2);

        pid_t child_two;
        child_two = fork();


        if (child_two < 0)
        {
        std::cerr << "Error: fork() failed";
        exit(-1);
        }
        else if (child_two == 0)
        {
            dup2(pipe2[0],STDIN_FILENO);
            dup2(pipe1[1],STDOUT_FILENO);

            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);

            char* arg[2];
            arg[0] = (char*)"ece650-a1.py";
            arg[1] = nullptr;
            execv ("ece650-a1.py", arg);
            perror ("Error from babaFin");
        }

        kids.push_back(child_two);

        dup2(pipe2[1],STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        char ss[init.s.size() + 1];
        init.s.copy(ss , init.s.size() +1 );
        ss[init.s.size()] = '\0';

        char nn[init.n.size() + 1];
        init.n.copy(nn , init.n.size() +1 );
        nn[init.n.size()] = '\0';

        char ll[init.l.size() + 1];
        init.l.copy(ll , init.l.size() +1 );
        ll[init.l.size()] = '\0';

        char cc[init.c.size() + 1];
        init.c.copy(cc , init.c.size() +1 );
        cc[init.c.size()] = '\0';

        char* argv[6];
        argv[0] = (char*)"rgen";
        argv[1] = (char*)ss;
        argv[2] = (char*)nn;
        argv[3] = (char*)ll;
        argv[4] = (char*)cc;
        argv[5] = nullptr;
        execv ("rgen", argv);
        perror ("Error from babaFin");
    }

    kids.push_back(child_one);

    pid_t child_three;

    child_three = fork();
    if (child_three < 0)
    {
        std::cerr << "Error: fork() failed";
        exit(-1);
    }
    else if(child_three == 0)
    {
        dup2(pipe1[0],STDIN_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);

        char* arr[2];
        arr[0] = (char*)"ece650-a2";
        arr[1] = nullptr;
        execv ("ece650-a2", arr);
        perror ("Error from babaFin");

    }

    kids.push_back(child_three);

    pid_t child_four;
    child_four=fork();
    if (child_four < 0)
    {
        std::cerr << "Error: fork() failed";
        exit(-1);
    }
    else if(child_four == 0)
    {
        dup2(pipe1[1],STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);

        char* arr[2];
        arr[0] = (char*)"reader";
        arr[1] = nullptr;
        execv ("reader", arr);
        perror ("Error from babaFin");

    }
    kids.push_back(child_four);


    close(pipe1[0]);
    close(pipe1[1]);

    int status;
    int status3;

    usleep(50000);
    child_one = waitpid(child_one, &status, WNOHANG);
    child_four = waitpid(child_four, &status3, WNOHANG);
   
    while ((child_four != -1) && (child_one != -1) )
    {
        usleep(50000);
        child_one = waitpid(child_one, &status, WNOHANG);
        child_four = waitpid(child_four, &status3, WNOHANG);
    }
    int status2;
    for (pid_t k : kids) {
        int status;
        kill (k, SIGTERM);
        waitpid(k, &status, 0);
    }
    return 0;
}
