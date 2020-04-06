#pragma once

#include <fstream>
#include <thread>
#include <iostream>

// For different algorithms PART
//___________________________________

// enum for different algorithms
enum class ECPU_Alg
{
    Default,            // Unrecognized algorithm
    Rise_Order          // In N-cores CPU 0 and N processor on the one core.
};

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
    ECPU_Alg const algorithm;

    CPU_info() : ready_cpus(std::thread::hardware_concurrency()), algorithm(GetCpuConfiguration()) {}

    // recieve propreate algorithm
    ECPU_Alg GetCpuConfiguration();
};

ECPU_Alg CPU_info::GetCpuConfiguration()
{
    char const * cpuifo = "/sys/bus/cpu/devices/cpu0/topology/core_cpus_list";

    std::ifstream cpuinfo{cpuifo, std::ios_base::in};
    if (!cpuinfo.is_open())
    {
        std::cerr << "Cann't open " << cpuifo << std::endl;
        return ECPU_Alg::Default;
    }

    int first = 0;
    int second = 0;
    char trash = 0;
    cpuinfo >> first >> trash >> second;

    cpuinfo.close();
    if ((second - first) == (ready_cpus / 2))
        return ECPU_Alg::Rise_Order;

    std::cerr << "Unrecognized cpu's algorithm!" << std::endl;
    return ECPU_Alg::Default;
}