#include "Thread.h"


int inputParser(int argc, char * argv);


int main(int argc, char * argv[])
{
    std::cout << Threads{inputParser(argc, argv[1])}.launch();
    return 0;
}


int inputParser(int argc, char * argv)
{
    if(argc == 1)
        return 1;
    else
        return strtol(argv, NULL, 10);
}