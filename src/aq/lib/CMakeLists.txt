
include_directories(. internal internal/linux ../../aqosa/lib ../../aqosa/lib/linux)
set(SOURCE
    AQ.cpp
    AQHeapMemory.cpp
    AQItem.cpp
    AQReader.cpp
    AQSharedMemoryWindow.cpp
    AQSnapshot.cpp
    AQUnformattedException.cpp
    AQWriter.cpp
    AQWriterItem.cpp
    internal/Crc32.cpp
    internal/CtrlOverlay.cpp
    internal/LinkedItemProcessor.cpp
    internal/TestPointNotifier.cpp
    internal/TraceBuffer.cpp
    internal/TraceManager.cpp
    internal/linux/AQWriterItem_linux.cpp
   )
add_library(aq STATIC ${SOURCE})
