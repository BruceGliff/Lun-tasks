#include "Task.h"
#include "cpu_optimization.h"

struct Threads : public Task
{
    pthread_t * threads = nullptr;
    CPU_info const info{};
    
    Threads(int threadsCount) : Task{threadsCount}, threads( (pthread_t *) calloc (threadsCount_, sizeof(pthread_t)) )
    {
        if(!info.isPropreateAlgorithm)
        {
            std::cerr << "Unrecognized CPU's configuration, pls send cpuinfo and core_cpu_list files" << std::endl;
            exit(-1);
        }
    }
    ~Threads() { free(threads); }

    double launch()
    {
        pthread_attr_t attr;
        int s = 0;
        s = pthread_attr_init(&attr);
        if (s)
        {
            std::cerr << "Cann't init attr" << std::endl;
            exit(-1);
        }

        for (int i = 0; i !=  threadsCount_; ++i)
        {
            s = set_stick_attr(i % info.ready_cpus, &attr);
            if (s)
            {
                std::cerr << "Cann't stick thread to current core" << std::endl;
                exit(-1);
            }

            s = pthread_create(&threads[i], &attr, Task::integrate, GetTask(i));
            if (s)
            {
                std::cerr << "Cann't create thread" << std::endl;
                exit(-1);
            }
        }

        double res = 0;
        for (int i = 0; i != threadsCount_; ++i)
        {   
            double out = 0;
            int const s = pthread_join(threads[i], NULL);
            if (s)
            {
                std::cerr << "Cann't join thread" << std::endl;
                exit(-1);
            }
            cacheLine * cache = reinterpret_cast<cacheLine *>(allCache_ + i * cacheSize);
            res += cache->out;
        }

        return res;
    }

    int set_stick_attr(int core_id, pthread_attr_t * attr) 
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
  
        return pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), &cpuset);
    }
};