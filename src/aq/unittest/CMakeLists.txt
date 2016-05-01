
include_directories(. ../../tst/lib ../../tst/lib/linux ../lib ../lib/internal ../lib/internal/linux)
set(SOURCE
    AQTest.cpp
    Main.cpp
    TestPointAction.cpp
    UtClaim.cpp
    UtCommit.cpp
    UtCrc32.cpp
    UtCrc32LinkId.cpp
    UtExtendable.cpp
    UtExtendableCrc32.cpp
    UtExtendableReader.cpp
    UtExtendableSnapshot.cpp
    UtExtendableWriter.cpp
    UtFormat.cpp
    UtObjectLifecycle.cpp
    UtQueueId.cpp
    UtRelease.cpp
    UtRetrieve.cpp
    UtSnapshot.cpp
    UtWriterItem.cpp
   )
add_executable(aq_unittest ${SOURCE})
target_link_libraries(aq_unittest aq tst rt)
