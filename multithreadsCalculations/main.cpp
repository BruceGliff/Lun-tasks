#include <iostream>
#include <cmath>
#include <string>
#include <thread>
#include <vector>

#include <unistd.h>

enum class Condition
{
    notWorkedYet,
    inWork,
    finished,
    checked,
};
struct cashLine
{
    double out;
    Condition status;
};
double const desc = 10e-6;

void integrate(double begin, double end, char * cash)
{
    cashLine * out = reinterpret_cast<cashLine *>(cash);
    out->status = Condition::inWork;
    double sum = 0;
    while (begin <= end)
    {
        sum += begin * begin * desc;
        begin += desc;
    }
    out->out = sum;
    out->status = Condition::finished;
}

int main(int argc, char * argv[])
{
    double begin = -10e3;
    double end = 10e3;

    int threadsCount = 0;
    int cashSize = 64;

    int maxThreads = std::thread::hardware_concurrency();

    double result = 0;
    if(argc == 1)
        threadsCount = 1;
    else
        threadsCount = strtol(argv[1], NULL, 10);

    double del = (end - begin) / threadsCount;
   
    char * allCash = (char *) ::operator new (cashSize * threadsCount);

    std::vector<std::thread *> queue{static_cast<std::size_t>(threadsCount - 1)};
    queue.reserve(threadsCount - 1);

    for(int i = 1; i != threadsCount; ++i)
    {
        std::thread * t = new std::thread{integrate, 
                                    begin + i * del,
                                    begin + (i + 1) * del,
                                    allCash + i * cashSize
                                    };
        t->detach();
        queue.push_back(t);
        
    }

    integrate(begin, begin + del, allCash);

    bool isFinished = false;
    while(!isFinished)
    {
        isFinished = true;
        for (int i = 0; i != threadsCount; ++i)
        {
            cashLine * out = reinterpret_cast<cashLine *>(allCash + i * cashSize);
            if (out->status == Condition::finished)
            {
                result += out->out;
                out->status = Condition::checked;
            }
            else if(out->status != Condition::checked)
                isFinished = false;
        }
    }

    std::cout << result << '\n';

    for (auto && x : queue)
        delete x;

    delete allCash;

    return 0;
}