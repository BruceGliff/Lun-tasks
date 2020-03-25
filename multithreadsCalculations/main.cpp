#include <iostream>
#include <cmath>
#include <string>
#include <thread>
#include <vector>

#include <unistd.h>

#include "cpu_optimization.h"

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
void integrate(double begin, double end, char * cache) noexcept;
struct MyThread
{
    // Task part
    double const begin = -10e3;
    double const end = 10e3;
    double del = 0.f;

    // Info about CPU
    CPU_info const info;

    // Preparation part
    char * allCache_ = nullptr;
    std::thread * queue_ = nullptr;
    int const cacheSize = 128;
    int currentThreads = 0;

    MyThread (int threadsCount) : info{threadsCount}
    {
        del = (end - begin) / threadsCount;
        allCache_ = new char[cacheSize * threadsCount];
        std::fill (allCache_, allCache_ + cacheSize * threadsCount, 0);

        if (threadsCount > 1)
        {
            queue_ = (std::thread *) ::operator new (sizeof(std::thread) * (threadsCount - 1), std::nothrow);
            if (!queue_)      
            {
                delete[] allCache_;
                throw std::bad_alloc{};
            }
        }
    }

    void Launch()
    {
        if (info.AlgToUse_ == EAlg::HyperThreads)
            stick_this_thread_to_core(0, pthread_self());

        for(int i = 0; i != info.usersThreads_ - 1; ++i)
        {  
            ++currentThreads;     
            new (&queue_[i]) std::thread    (std::thread{integrate, 
                                            begin + i * del,
                                            begin + (i + 1) * del,
                                            allCache_ + i * cacheSize + deltaCache
                                            });

            if (i < info.threads_)
                stick_this_thread_to_core(i + 1, queue_[i].native_handle());

            queue_[i].detach(); 
        }
    }
    void LaunchMain() const noexcept
    {
        integrate   (begin + (info.usersThreads_ - 1) * del, 
                    begin + info.usersThreads_ * del, 
                    allCache_ + (info.usersThreads_ - 1) * cacheSize + deltaCache);
    }

    double Count() noexcept
    {
        double result = 0;
        while (!areThreadsDone()) {}

        for (int i = 0; i != info.usersThreads_; ++i)
        {
            cacheLine * out = reinterpret_cast<cacheLine *>(allCache_ + i * cacheSize + deltaCache);
            result += out->out;
        } 

        return result;
    }

    bool areThreadsDone() noexcept
    {
        for (int i = 0; i != currentThreads; ++i)
        {
            cacheLine * out = reinterpret_cast<cacheLine *>(allCache_ + i * cacheSize + deltaCache);
            if (out->status == Condition::finished)
                out->status == Condition::checked;
            else if(out->status != Condition::checked)
                return false;            
        }
        return true;
    }

    ~MyThread()
    {
        while(!areThreadsDone()) {}

        for (int i = 0; i != currentThreads; ++i)
            queue_[i].~thread();

        delete[] allCache_;
        if (info.usersThreads_ > 1)
            delete queue_;
    }
};

void integrate(double begin, double end, char * cache) noexcept
{
    double const desc = 10e-6;
    cacheLine * out = reinterpret_cast<cacheLine *>(cache);
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
    if (!threadsCount)
        return 0;

    MyThread * Thread = nullptr;

    try { 
        Thread = new MyThread{threadsCount};
    } 
    catch (std::bad_alloc const & e)
    { std::cerr << "Bad alloc: \n" << e.what(); return 0; }

    try{
    Thread->Launch();
    } catch (std::system_error const & e)
    { std::cerr << "System error: \n" << e.what(); delete Thread; return 0; }

    Thread->LaunchMain();

    result = Thread->Count();

    std::cout << result << '\n';

    delete Thread;

    return 0;
}