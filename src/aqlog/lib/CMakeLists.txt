
include_directories(. internal internal/linux)
set(SOURCE
    internal/LLHash.cpp
   )
add_library(aqlog STATIC ${SOURCE})
