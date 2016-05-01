
include_directories(. Internal Internal/Linux)
set(SOURCE
    AQ.cpp
    AQItem.cpp
    AQReader.cpp
    AQSnapshot.cpp
    AQUnformattedException.cpp
    AQWriter.cpp
    AQWriterItem.cpp
    Internal/Crc32.cpp
    Internal/CtrlOverlay.cpp
    Internal/LinkedItemProcessor.cpp
    Internal/TestPointNotifier.cpp
    Internal/Timer.cpp
    Internal/TraceBuffer.cpp
    Internal/TraceManager.cpp
    Internal/Linux/AQWriterItem_vprintf.cpp
   )
add_library(AQ STATIC ${SOURCE})
