
include_directories(. ../TestLib ../TestLib/Linux ../AQ ../AQ/Internal ../AQ/Internal/Linux)
set(SOURCE
    ConsumerChannel.cpp
    ItemGenerator.cpp
    Main.cpp
    Producer.cpp
    SnapshotTaker.cpp
    SnapshotValidator.cpp
   )
add_executable(StressTest ${SOURCE})
target_link_libraries(StressTest AQ TestLib pthread rt)
