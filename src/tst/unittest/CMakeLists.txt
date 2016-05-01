
include_directories(. ../lib ../lib/linux)
set(SOURCE
    Main.cpp
    UtTest.cpp
   )
add_executable(tst_unittest ${SOURCE})
target_link_libraries(tst_unittest tst rt)
