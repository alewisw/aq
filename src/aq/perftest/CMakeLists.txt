
include_directories(. ../../tst/lib ../../tst/lib/linux ../..aqosa/lib ../..aqosa/lib/linux ../lib ../lib/internal ../lib/internal/linux)
set(SOURCE
    AQProvider.cpp
    ClaimCommitTest.cpp
    ClaimTest.cpp
    CommitTest.cpp
    FullQueueTest.cpp
    Main.cpp
    PerfTest.cpp
    QueueTest.cpp
    ReleaseTest.cpp
    RetrieveReleaseTest.cpp
    RetrieveTest.cpp
    ThreadOverheadTest.cpp
   )
add_executable(aq_perftest ${SOURCE})
target_link_libraries(aq_perftest aq aqosa tst pthread rt)
