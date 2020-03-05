#include <iostream>
#include <cmath>
#include <string>
#include <thread>
#include <vector>

#include <unistd.h>

constexpr int deltaCache = 32;

enum class Condition
{
    notWorkedYet,
    inWork,
    finished,
    checked,
};
struct cacheLine
{
    double out;
    Condition status;
};
double const desc = 10e-2;//-6

struct MyThread
{
    char * allCache_ = nullptr;
    std::thread * queue_ = nullptr;
    int cacheSize = 128;
    int threadsCount_ = 0;

    MyThread (int threadsCount) : threadsCount_(threadsCount)
    {
        allCache_ = new char[cacheSize * threadsCount_];

        if (threadsCount_ > 1)
        {
            try
            {
                queue_ = (std::thread *) ::operator new (sizeof(std::thread) * (threadsCount - 1));       
            }
            catch(std::bad_alloc const & e)
            {
                delete[] allCache_;
                std::cerr << e.what() << '\n';
                throw;
            }
        }
    }

    void Launch()
    {
        
    }

    ~MyThread()
    {
        delete[] allCache_;
        if (threadsCount_ > 1)
            delete queue_;
    }
};

void integrate(double begin, double end, char * cache)
{
    cacheLine * out = reinterpret_cast<cacheLine *>(cache + deltaCache);
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
    int cacheSize = 128;

    int maxThreads = std::thread::hardware_concurrency();

    double result = 0;
    if(argc == 1)
        threadsCount = 1;
    else
        threadsCount = strtol(argv[1], NULL, 10);

    double del = (end - begin) / threadsCount;
    // char * allcache = nullptr;
    // std::thread * queue = nullptr;

    extern MyThread Thread;
   
    try { 
        MyThread{threadsCount};
        // allcache = new char[cacheSize * threadsCount]; 
        // queue = (std::thread *) ::operator new (sizeof(std::thread) * (threadsCount - 1));
    } 
    catch (std::bad_alloc const & e)
    { std::cerr << "Bad alloc: \n" << e.what(); return 0; }

    try{
    for(int i = 0; i != threadsCount - 1; ++i)
    {        
        new (&queue[i]) std::thread (std::thread{integrate, 
                                    begin + (i + 1) * del,
                                    begin + (i + 2) * del,
                                    allcache + (i + 1) * cacheSize
                                    });

        //new (pointer) T (value);


        queue[i].detach();
        
    }}
    catch (std::bad_alloc const & e)
    { std::cerr << "Bad alloc: \n" << e.what(); return 0;}

    integrate(begin, begin + del, allcache);

    bool isFinished = false;
    while(!isFinished)
    {
        isFinished = true;
        for (int i = 0; i != threadsCount; ++i)
        {
            cacheLine * out = reinterpret_cast<cacheLine *>(allcache + i * cacheSize + deltaCache);
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

    for (int i = 0; i != threadsCount - 1; ++i)
        queue[i].~thread();
    
    delete queue;
    delete[] allcache;

    return 0;
}