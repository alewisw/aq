
include_directories(. linux)
set(SOURCE
    ProcessIdentifier.cpp
    Timer.cpp
    Timestamp.cpp
   )
add_library(aqosa STATIC ${SOURCE})
