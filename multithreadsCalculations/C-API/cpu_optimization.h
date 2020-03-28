#pragma once

#include <fstream>
#include <thread>
#include <iostream>

// For different algorithms PART
//___________________________________
struct Condition
{
    int core_id = 0;
    bool isBusy = false;
};

struct Pair
{
    Condition first;
    Condition second;
};
//__________________________________

struct CPU_info
{
    int const ready_cpus;
    bool const isPropreateAlgorithm;

    CPU_info() : ready_cpus(std::thread::hardware_concurrency()), isPropreateAlgorithm(GetCpuConfiguration()) {}


    bool GetCpuConfiguration();
};

bool CPU_info::GetCpuConfiguration()
{
    char const * cpuifo = "/sys/bus/cpu/devices/cpu0/topology/core_cpus_list";

    std::ifstream cpuinfo{cpuifo, std::ios_base::in};
    if (!cpuinfo.is_open())
    {
        std::cerr << "Cann't open " << cpuifo << std::endl;
        return false;
    }

    int first = 0;
    int second = 0;
    char trash = 0;
    cpuinfo >> first >> trash >> second;

    cpuinfo.close();
    return (second - first) == (ready_cpus / 2);
}