#include "Task.h"
#include "cpu_optimization.h"

#include <mutex>

namespace LOCK
{
    std::mutex isEnd;
}

struct Threads : public Task
{
    // memory for threads
    pthread_t * threads = nullptr;
    // general info 
    CPU_info const info{};
    
    // Create Task discription and memory for threads
    Threads(int threadsCount) : Task{threadsCount}, threads( (pthread_t *) calloc (threadsCount_ < info.ready_cpus / 2 ? info.ready_cpus / 2 : threadsCount_, sizeof(pthread_t)) ) {}
    ~Threads() { free(threads); }

    // Start calculating task
    double launch()
    {
        //--------------------------
        //  init pthread parametrs
        pthread_attr_t attr;
        int s = 0;
        s = pthread_attr_init(&attr);
        if (s)
        {
            std::cerr << "Cann't init attr" << std::endl;
            exit(-1);
        }


        //--------------------------
        //  chose propreate algorithm
        switch (info.algorithm)
        {
        case ECPU_Alg::Rise_Order:
            for (int i = 0; i !=  threadsCount_; ++i)
            {
                set_stick_attr(i % info.ready_cpus, &attr);
                threadCreate(i, attr);
            }
            break;
        case ECPU_Alg::Default:
            for (int i = 0; i !=  threadsCount_; ++i)
                threadCreate(i, attr);
            break;
        
        default:
            std::cerr << "Unexpected program behavior!" << std::endl;
            exit(-1);
            break;
        }

        //--------------------------
        //  create trash threads to hyperthread
        LOCK::isEnd.lock();
        for (int i = threadsCount_; i < info.ready_cpus / 2; ++i)
        {
            set_stick_attr(i, &attr);
            int const s = pthread_create(&threads[i], &attr, Threads::TrashTask, NULL);
            if (s)
            {
                std::cerr << "Cann't create thread" << std::endl;
                exit(-1);
            }
            int const s1 = pthread_detach(threads[i]);
            if (s1)
            {
                std::cerr << "Cann't detach thread" << std::endl;
                exit(-1);
            }
        }

       
        //--------------------------
        //   waiting till calculations are done
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

        //--------------------------
        //  waiting till trash is done
        LOCK::isEnd.unlock();

        return res;
    }


    // Creating thread with param in attr
    int threadCreate(int indx, pthread_attr_t const & attr)
    {
        int const s = pthread_create(&threads[indx], &attr, Task::integrate, GetTask(indx));
        if (s)
        {
            std::cerr << "Cann't create thread" << std::endl;
            exit(-1);
        }

        return 0;
    }


    // Stick thread to specific core
    int set_stick_attr(int core_id, pthread_attr_t * attr) 
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
  
        int s = pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), &cpuset);
        if (s)
        {
            std::cerr << "Cann't stick thread to current core" << std::endl;
            exit(-1);
        }
        
        return 0;
    }

    // Task to make free thread busy
    static void* TrashTask(void * finish)
    {
        while(!LOCK::isEnd.try_lock()) {}
        LOCK::isEnd.unlock();

        return NULL;
    }
};