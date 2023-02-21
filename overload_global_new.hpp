#ifndef HEADER_INCLUSION_GUARD_OVERLOAD_GLOBAL_NEW_HPP
#define HEADER_INCLUSION_GUARD_OVERLOAD_GLOBAL_NEW_HPP

#include <cstddef> // size_t, max_align_t
#include <cstdlib> // malloc
#include <new>     // required by g++ otherwise we get error for 'noexcept'

std::size_t g_total_allocation = 0u;

inline void *Implementation_Global_New(std::size_t size) noexcept
{
    using std::size_t;

    size += alignof(std::max_align_t) - (size % alignof(std::max_align_t));

    static size_t constexpr bytes_at_a_time = 10485760u; /* We allocate 10 megabytes at a time */

    if ( size > bytes_at_a_time ) return nullptr;

    static void *p = nullptr;

    static size_t bytes_allocated_so_far = 0u;

    for (; /* ever */ ;)
    {
        if ( nullptr == p )
        {
            p = std::malloc(bytes_at_a_time);

            if ( nullptr == p ) return nullptr;

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

void *operator new  (std::size_t const size) noexcept { return Implementation_Global_New(size); }
void *operator new[](std::size_t const size) noexcept { return Implementation_Global_New(size); }
void operator delete  (void *const p) noexcept { /* Do Nothing */ }
void operator delete[](void *const p) noexcept { /* Do Nothing */ }

#endif
