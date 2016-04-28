
include_directories(. ../TestLib ../TestLib/Linux ../AQ ../AQ/Internal ../AQ/Internal/Linux)
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
    UtObjectCopyAssign.cpp
    UtQueueId.cpp
    UtRelease.cpp
    UtRetrieve.cpp
    UtSnapshot.cpp
    UtTest.cpp
    UtWriterItem.cpp
   )
add_executable(UnitTest ${SOURCE})
target_link_libraries(UnitTest AQ TestLib rt)
