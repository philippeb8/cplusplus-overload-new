/*
    Compile and run with:
    
    $ g++ -O2 test.cpp -o test -lallocator -L.
    $ LD_LIBRARY_PATH=. ./test
    
    
    Note:
    
    "clang++ -O2" gets rid of the operator overrides
    
    
    Output without shared library:
    
    Speed: 0.0297943 blocks / nanosecond; Memory usage: 2612 kilobytes
    
    
    Output with shared library (~6x faster):
    
    Speed: 0.129081 blocks / nanosecond; Memory usage: 66096 kilobytes
*/

#include <stdlib.h>
#include <sys/resource.h>

#include <chrono>
#include <memory>
#include <iostream>

using namespace std;

int main()
{
    for (auto timestamp = chrono::system_clock::now(), tick = timestamp; chrono::duration_cast<chrono::minutes>(chrono::system_clock::now() - timestamp).count() < 2; tick = chrono::system_clock::now())
    {
        for (size_t i = 0; i < 1024; ++ i)
        {
            char * p = new char[1024];
            
            delete [] p;
        }
        
        // stats
        struct rusage r_usage;
        getrusage(RUSAGE_SELF, & r_usage);

        cout << "Speed: " << double(1024) / chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now() - tick).count() << " blocks / nanosecond" << "; Memory usage: " << r_usage.ru_maxrss << " kilobytes" << endl;
    }
    
    return 0;
}
