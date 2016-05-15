
include_directories(. ../../tst/lib ../../tst/lib/linux ../../aqosa/lib ../../aqosa/lib/linux ../lib ../lib/internal ../lib/internal/linux)
set(SOURCE
    ConsumerChannel.cpp
    ItemGenerator.cpp
    Main.cpp
    Producer.cpp
    SnapshotTaker.cpp
    SnapshotValidator.cpp
   )
add_executable(aq_stresstest ${SOURCE})
target_link_libraries(aq_stresstest aq aqosa tst pthread rt)
