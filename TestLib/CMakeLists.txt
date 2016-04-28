
include_directories(. Linux ../AQ ../AQ/Internal ../AQ/Internal/Linux)
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
    Linux/Event.cpp
    Linux/WorkerThread.cpp
   )
add_library(TestLib STATIC ${SOURCE})
