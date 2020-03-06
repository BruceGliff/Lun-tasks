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
void integrate(double begin, double end, char * cache) noexcept;
struct MyThread
{
    // Task part
    double begin = -10e3;
    double end = 10e3;
    double del = 0.f;


    char * allCache_ = nullptr;
    std::thread * queue_ = nullptr;
    int cacheSize = 128;
    int threadsCount_ = 0;
    int maxThreads = 0;
    int currentThreads = 0;

    MyThread (int threadsCount) : threadsCount_(threadsCount), maxThreads(std::thread::hardware_concurrency())
    {
        del = (end - begin) / threadsCount_;
        allCache_ = new char[cacheSize * threadsCount_];
        std::fill (allCache_, allCache_ + cacheSize * threadsCount_, 0);

        if (threadsCount_ > 1)
        {
            queue_ = (std::thread *) ::operator new (sizeof(std::thread) * (threadsCount_ - 1), std::nothrow);
            if (!queue_)      
            {
                delete[] allCache_;
                throw std::bad_alloc{};
            }
        }
    }

    void Launch()
    {
        for(int i = 0; i != threadsCount_ - 1; ++i)
        {  
            ++currentThreads;     
            new (&queue_[i]) std::thread    (std::thread{integrate, 
                                            begin + i * del,
                                            begin + (i + 1) * del,
                                            allCache_ + i * cacheSize + deltaCache
                                            });
            queue_[i].detach(); 
        }
    }
    void LaunchMain() const noexcept
    {
        integrate   (begin + (threadsCount_ - 1) * del, 
                    begin + threadsCount_ * del, 
                    allCache_ + (threadsCount_ - 1) * cacheSize + deltaCache);
    }

    double Count() noexcept
    {
        double result = 0;
        while (!areThreadsDone()) {}

        for (int i = 0; i != threadsCount_; ++i)
        {
            cacheLine * out = reinterpret_cast<cacheLine *>(allCache_ + i * cacheSize + deltaCache);
            result += out->out;
        } 

        return result;
    }

    bool areThreadsDone()
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
        if (threadsCount_ > 1)
            delete queue_;
    }
};

void integrate(double begin, double end, char * cache) noexcept
{
    double const desc = 10e-6;//-6
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

    MyThread * Thread;
   
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