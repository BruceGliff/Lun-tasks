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
void integrate(double begin, double end, char * cache);
struct MyThread
{
    // Task part
    double begin = -10e3;
    double end = 10e3;
    int maxThreads;
    double del;


    char * allCache_ = nullptr;
    std::thread * queue_ = nullptr;
    int cacheSize = 128;
    int deltaCache = 32;
    int threadsCount_ = 0;


    MyThread (int threadsCount) : threadsCount_(threadsCount), maxThreads(std::thread::hardware_concurrency())
    {
        del = (end - begin) / threadsCount_;
        allCache_ = new char[cacheSize * threadsCount_];

        if (threadsCount_ > 1)
        {
            try
            {
                queue_ = (std::thread *) ::operator new (sizeof(std::thread) * (threadsCount_ - 1));       
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
        try{
        for(int i = 0; i != threadsCount_ - 1; ++i)
        {        
            new (&queue_[i]) std::thread    (std::thread{integrate, 
                                            begin + (i + 1) * del,
                                            begin + (i + 2) * del,
                                            allCache_ + (i + 1) * cacheSize
                                            });

            queue_[i].detach();
        }
        }
        catch (std::bad_alloc const & e)
        { std::cerr << e.what() << '\n'; }
    }
    void LaunchMain() const noexcept
    {
        integrate(begin, begin + del, allCache_);
    }

    double Count()
    {
        double result = 0;
        bool isFinished = false;
        while(!isFinished)
        {
            isFinished = true;
            for (int i = 0; i != threadsCount_; ++i)
            {
                cacheLine * out = reinterpret_cast<cacheLine *>(allCache_ + i * cacheSize + deltaCache);
                if (out->status == Condition::finished)
                {
                    result += out->out;
                    out->status = Condition::checked;
                }
                else if(out->status != Condition::checked)
                    isFinished = false;
            } 
        }
        return result;
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
    double const desc = 10e-6;//-6
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
    int threadsCount = 0;
    double result = 0;
    if(argc == 1)
        threadsCount = 1;
    else
        threadsCount = strtol(argv[1], NULL, 10);

    MyThread * Thread;
   
    try { 
        Thread = new MyThread{threadsCount};
    } 
    catch (std::bad_alloc const & e)
    { std::cerr << "Bad alloc: \n" << e.what(); return 0; }

    try
    {
        Thread->Launch();
    }
    catch (std::bad_alloc const & e)
    { std::cerr << "Bad alloc: \n" << e.what(); return 0;}

    Thread->LaunchMain();

    result = Thread->Count();

    std::cout << result << '\n';

    delete Thread;

    return 0;
}