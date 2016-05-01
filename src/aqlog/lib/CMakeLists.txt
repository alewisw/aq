
include_directories(. internal internal/linux)
set(SOURCE
    AQ.cpp
    AQItem.cpp
    AQReader.cpp
    AQSnapshot.cpp
    AQUnformattedException.cpp
    AQWriter.cpp
    AQWriterItem.cpp
    internal/Crc32.cpp
    internal/CtrlOverlay.cpp
    internal/LinkedItemProcessor.cpp
    internal/TestPointNotifier.cpp
    internal/Timer.cpp
    internal/TraceBuffer.cpp
    internal/TraceManager.cpp
    internal/linux/AQWriterItem_vprintf.cpp
   )
add_library(aq STATIC ${SOURCE})
