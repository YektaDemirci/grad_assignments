#include<iostream>
#include<fstream>
#include<vector>
#include<unistd.h>

int main(int argc, char *argv[])
{
    while (!std::cin.eof())
    {
        std::string inp;
        std::getline(std::cin, inp);
        // read a line of input until EOL and store in a string
        if (inp.size () > 0)
        {
            std::cout << inp << std::endl;
        }
    }

    return -1;
}
