#include <fstream>
#include <string>
#include <iostream>

int main()
{
    std::ifstream file("loglistener.txt");
    std::string line;
    std::string own_addr = "Own_addr";

    while (std::getline(file, line))
    {
        if (line.find(own_addr) != std::string::npos)
        {
            printf("%s\n", line.c_str());
        }
    }
}
