.PHONY : clean

CC=g++
CFLAGS=-std=c++17 -O2 -fcheck-new

all: liballocator_static.so liballocator_threadlocal.so test_static_system test_static_user test_threadlocal_system test_threadlocal_user

clean:
	rm -f *.so *.o test_static_system test_static_user test_threadlocal_system test_threadlocal_user

allocator_static.o: allocator.cpp
	$(CC) $(CFLAGS) -fPIC -c -o $@ $< -DTHREADLOCAL=0

allocator_threadlocal.o: allocator.cpp
	$(CC) $(CFLAGS) -fPIC -c -o $@ $< -DTHREADLOCAL=1

lib%.so: %.o
	$(CC) -shared -o $@ $<

test_static_system: test.cpp
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

test_static_user: test.cpp liballocator_static.so
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lallocator_static -L.

test_threadlocal_system: test.cpp
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

test_threadlocal_user: test.cpp liballocator_threadlocal.so
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lallocator_threadlocal -L.
