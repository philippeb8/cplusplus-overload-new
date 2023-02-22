/*
    Fast garbage collector-style memory allocator.
    
    Idea from: Frederick Virchanza Gotham
*/

#include <cstddef> // size_t, max_align_t
#include <cstdlib> // malloc

#include <new>     // required by g++ otherwise we get error for 'noexcept'
#include <atomic>
#include <thread>

#if THREADLOCAL

#define STORAGE thread_local

template <typename T>
    using atom = T;
    
#else
    
#define STORAGE static
    
template <typename T>
    using atom = std::atomic<T>;
    
#endif


template <size_t S = 10485760u> /* We allocate 10 megabytes at a time */
    void *Implementation_Global_New(std::size_t size) noexcept
    {
        using std::size_t;

        size += alignof(std::max_align_t) - (size % alignof(std::max_align_t));

        STORAGE atom<std::size_t> g_total_allocation = 0u;
        
        STORAGE atom<size_t> constexpr bytes_at_a_time = S;

        if ( size > bytes_at_a_time ) 
            return nullptr;

        STORAGE atom<void *> p = nullptr;

        STORAGE atom<size_t> bytes_allocated_so_far = 0u;

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

        void *const retval = static_cast<char*>(static_cast<void *>(p)) + bytes_allocated_so_far;

        bytes_allocated_so_far += size;

        return retval;
    }

// no inline, required by [replacement.functions]/3
void* operator new(std::size_t sz) throw()
{
    if (void *ptr = Implementation_Global_New<>(sz))
        return ptr;
        
    //throw std::bad_alloc{}; // required by [new.delete.single]/3
    
    return nullptr;
}

// no inline, required by [replacement.functions]/3
void* operator new[](std::size_t sz) throw()
{
    if (void *ptr = Implementation_Global_New<>(sz))
        return ptr;
        
    //throw std::bad_alloc{}; // required by [new.delete.single]/3

    return nullptr;
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
