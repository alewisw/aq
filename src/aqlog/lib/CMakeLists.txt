
include_directories(. internal internal/linux ../../aqosa/lib ../../aqosa/lib/linux)
set(SOURCE
    internal/LLHash.cpp
   )
add_library(aqlog STATIC ${SOURCE})
