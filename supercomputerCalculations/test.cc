#include <iostream>

#include "Task.h"

int main()
{
    TasksQueue q;
    int a = 1;
    do
    {   
        Task t;
        a = q.getTask(&t);
        std::cout << t.begin << ' ' << t.end << ' ' << t.del << ' ' << t.res << std::endl;


    } while (!a);
    

}