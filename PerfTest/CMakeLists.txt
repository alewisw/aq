
include_directories(. ../TestLib ../TestLib/Linux ../AQ ../AQ/Internal ../AQ/Internal/Linux)
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
add_executable(PerfTest ${SOURCE})
target_link_libraries(PerfTest AQ TestLib pthread rt)
