#!/bin/bash

export LD_LIBRARY_PATH=.

set -x

./test_static_system 
./test_static_user 
./test_threadlocal_system 
./test_threadlocal_user
