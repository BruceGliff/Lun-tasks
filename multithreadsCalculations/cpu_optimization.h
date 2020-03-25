#pragma once

// core_id = 0, 1, ... n-1, where n is the system's number of cores
//#define _GNU_SOURCE 
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include <iostream>
#include <string>
#include <fstream>
#include <thread>

enum class EAlg
{
    Automitic,
    Optimal,
    HyperThreads
};


class CPU_info
{
public:
    int const cores_;
    int const threads_;

    int usersThreads_;
    EAlg AlgToUse_;

    static int get_cpu_cores() noexcept;

    CPU_info(int threadsNumber) : cores_(get_cpu_cores()), threads_(std::thread::hardware_concurrency()), usersThreads_(threadsNumber) 
    {
        if (usersThreads_ <= cores_)
        {
            AlgToUse_ = EAlg::HyperThreads;
            return;
        }
        if (usersThreads_ < threads_)
        {
            AlgToUse_ = EAlg::Optimal;
            return;
        }
        AlgToUse_ = EAlg::Automitic;
    }
};

int CPU_info::get_cpu_cores() noexcept
{
    char const * cpuifo = "/proc/cpuinfo";

    std::ifstream cpuinfo{cpuifo, std::ios_base::in};
    if (!cpuinfo.is_open())
    {
        std::cerr << "Cann't open " << cpuifo << '\n';
        cpuinfo.close();
        return std::thread::hardware_concurrency() / 2;
    }
    std::string in;
    std::string next_in;

    while (cpuinfo >> in)
    {
        if (in.compare("cpu") == 0)
        {
            cpuinfo >> next_in;
            if (next_in.compare("cores") == 0)
            {
                int cores = 0;
                char trash = 0;
                cpuinfo >> trash >> cores;
                cpuinfo.close();
                return cores;
            }
        }
    }

    std::cerr << "Can't find info about cpu in" << cpuifo << '\n';
    cpuinfo.close();
    return -1;
}

int stick_this_thread_to_core(int core_id, pthread_t threadToStick) 
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
  
    return pthread_setaffinity_np(threadToStick, sizeof(cpu_set_t), &cpuset);
}