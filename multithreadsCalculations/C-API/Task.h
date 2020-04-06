# pragma once
#include <iostream>

constexpr int MAX_THREADS = 144;

struct cacheLine
{
    double out;
    double begin;
    double end;
};

struct Task
{
    int     const cacheSize = 128;
    double  const begin = -5e3;
    double  const end = 5e3;
    double        del = 0.f;
    char *        allCache_ = nullptr;
    int           threadsCount_ = 0;

    static void* integrate(void * cache);

    Task(int threadsCount)
    {
        threadsCount_ = threadsCount < MAX_THREADS ? threadsCount : MAX_THREADS; 

        del = (end - begin) / threadsCount_;
        allCache_ = new(std::nothrow) char[cacheSize * threadsCount_];
        if (!allCache_)
        {
            std::cerr << "Cann't allocate memory for cache size of:" << cacheSize * threadsCount_ << std::endl;
            exit(-1);
        }
    }
    ~Task() { delete[] allCache_; }

    void * GetTask(int i)
    {
        cacheLine * task = reinterpret_cast<cacheLine *>(allCache_ + i * cacheSize);
        task->begin = begin + i * del;
        task->end = begin + (i + 1) * del;
        return reinterpret_cast<void *>(task);
    }
};


void* Task::integrate(void * cache)
{
    double const desc = 10e-6;
    cacheLine * out = reinterpret_cast<cacheLine *>(cache);
    double sum = 0;
    while (out->begin <= out->end)
    {
        sum += out->begin * out->begin * desc;
        out->begin += desc;
    }
    out->out = sum;

}