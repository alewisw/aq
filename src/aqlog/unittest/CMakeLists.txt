
include_directories(. ../../tst/lib ../../tst/lib/linux  ../../aqosa/lib ../../aqosa/lib/linux ../lib ../lib/internal ../lib/internal/linux)
set(SOURCE
    Main.cpp
    UtMacroTricks.cpp
   )
add_executable(aqlog_unittest ${SOURCE})
target_link_libraries(aqlog_unittest aq aqlog aqosa tst rt)
