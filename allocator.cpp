/*
    Compile with:
    
    $ g++ -O2 -fPIC -c -o allocator.o allocator.cpp
    $ g++ -shared -o liballocator.so allocator.o
*/

#include <cstddef> // size_t, max_align_t
#include <cstdlib> // malloc
#include <stdio.h>
#include <new>     // required by g++ otherwise we get error for 'noexcept'

std::size_t g_total_allocation = 0u;

inline void *Implementation_Global_New(std::size_t size) noexcept
{
    using std::size_t;

    size += alignof(std::max_align_t) - (size % alignof(std::max_align_t));

    static size_t constexpr bytes_at_a_time = 10485760u; /* We allocate 10 megabytes at a time */

    if ( size > bytes_at_a_time ) 
        return nullptr;

    static void *p = nullptr;

    static size_t bytes_allocated_so_far = 0u;

    for (; /* ever */ ;)
    {
        if ( nullptr == p )
        {
            p = std::malloc(bytes_at_a_time);

            if ( nullptr == p ) 
                return nullptr;

            g_total_allocation += bytes_at_a_time;
        }

        if ( (bytes_allocated_so_far + size) > bytes_at_a_time )
        {
            p = nullptr;

            bytes_allocated_so_far = 0u;
        }
        else
        {
            break;
        }
    }

    void *const retval = static_cast<char*>(p) + bytes_allocated_so_far;

    bytes_allocated_so_far += size;

    return retval;
}

// no inline, required by [replacement.functions]/3
void* operator new(std::size_t sz)
{
    if (void *ptr = Implementation_Global_New(sz))
        return ptr;
        
    throw std::bad_alloc{}; // required by [new.delete.single]/3
}

// no inline, required by [replacement.functions]/3
void* operator new[](std::size_t sz)
{
    if (void *ptr = Implementation_Global_New(sz))
        return ptr;
        
    throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void operator delete(void* ptr) noexcept
{
}

void operator delete(void* ptr, std::size_t size) noexcept
{
}

void operator delete[](void* ptr) noexcept
{
}

void operator delete[](void* ptr, std::size_t size) noexcept
{
}
