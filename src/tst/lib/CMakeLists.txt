
include_directories(. linux ../../aq/lib ../../aq/lib/internal ../../aq/lib/internal/linux)
set(SOURCE
    AQStrawMan.cpp
    Optarg.cpp
    Prng.cpp
    TestAssert.cpp
    TestExecution.cpp
    TestJUnitXmlReport.cpp
    TestRunner.cpp
    TestSuite.cpp
    TestTag.cpp
    linux/Event.cpp
    linux/WorkerThread.cpp
   )
add_library(tst STATIC ${SOURCE})
