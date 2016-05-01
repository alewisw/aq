cmake_minimum_required(VERSION 2.6)
project(aq)

set(GCC_FLAGS "-fexceptions -Wall -Wextra -Werror -Wno-parentheses -Wno-unused-parameter -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS")
set(GCC_PERF_FLAGS "-fmodulo-sched -fmodulo-sched-allow-regmoves -fgcse-sm -fgcse-las -fgcse-after-reload -funsafe-loop-optimizations -Wunsafe-loop-optimizations -fivopts -funroll-loops")
set(CMAKE_CXX_FLAGS_DEBUG "${GCC_FLAGS} -DAQ_TEST_POINT -DAQ_TEST_TRACE  -DAQ_TEST_UNIT -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_CXX_FLAGS_RELEASE "${GCC_FLAGS} ${GCC_PERF_FLAGS} -DAQ_TEST_POINT -DAQ_TEST_TRACE -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_COVERAGE "${GCC_FLAGS} -DAQ_TEST_POINT -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_CXX_FLAGS_PERFORMANCE "${GCC_FLAGS} ${GCC_PERF_FLAGS} -O3 -DNDEBUG")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE}")
set(CMAKE_C_FLAGS_PERFORMANCE "${CMAKE_CXX_FLAGS_PERFORMANCE}")

set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "")
set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "")
set(CMAKE_EXE_LINKER_FLAGS_PERFORMANCE "")
set(CMAKE_SHARED_LINKER_FLAGS_PERFORMANCE "")

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_PERFORMANCE
    CMAKE_C_FLAGS_PERFORMANCE
    CMAKE_EXE_LINKER_FLAGS_PERFORMANCE
    CMAKE_SHARED_LINKER_FLAGS_PERFORMANCE)

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE)
    
    
add_subdirectory(src/aq/lib)
add_subdirectory(src/aq/perftest)
add_subdirectory(src/aq/stresstest)
add_subdirectory(src/aq/unittest)
add_subdirectory(src/tst/lib)
add_subdirectory(src/tst/unittest)
