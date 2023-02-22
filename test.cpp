/*
    Memory allocators benchmarks.

    Note:
    "clang++ -O2" optimization gets rid of the operator overrides
    
    Author:
    Phil Bouchard
*/

#include <stdlib.h>
#include <sys/resource.h>

#include <vector>
#include <chrono>
#include <memory>
#include <iostream>
#include <thread>


using namespace std;


void task(size_t s, size_t b)
{
    vector<char *> v(s);
    
    for (size_t i = 0; i < s; ++ i)
        v[i] = new char[b];

    for (size_t i = 0; i < s; ++ i)
        delete [] v[i];
}

int main()
{
    for (auto timestamp = chrono::system_clock::now(), tick = timestamp; chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - timestamp).count() < 1; tick = chrono::system_clock::now())
    {
        thread pool[16];
        
        for (auto i = pool; i < pool + sizeof(pool) / sizeof(*pool); ++i)
            *i = move(thread(task, 256 * 1024, 16));
        
        for (auto i = pool; i < pool + sizeof(pool) / sizeof(*pool); ++i)
            i->join();
        
        // stats
        struct rusage r_usage;
        getrusage(RUSAGE_SELF, & r_usage);

        cout << "Speed: " << double(256 * 1024) / chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now() - tick).count() << " blocks / nanosecond" << "; Memory usage: " << r_usage.ru_maxrss << " kilobytes" << endl;
    }
    
    return 0;
}
