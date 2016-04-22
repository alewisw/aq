#ifndef TESTRUNNER_H
#define TESTRUNNER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <map>
#include <vector>
#include <string>



//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class TestAssert;
class TestExecution;
class TestTag;
class TestSuite;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// A TestRunner is used to run all the test cases that have been declared as 
// part of a unit test program.
class TestRunner
{

public:

    // Constructs a new test runner.
    TestRunner(int argc, char* argv[]);

    // Copy and assignment not supported - not implemented.
    TestRunner(const TestRunner& other);
    TestRunner& operator=(const TestRunner& other);

    // Destroys this test runner.
    virtual ~TestRunner(void);

private:

    // Adds the passed 'suite' to the 'm_suites' vector and the 'm_fileToSuite' map.
    void addSuite(TestSuite *suite);

    // Adds an executor for 'tag' to the suite that it matches in the 'm_fileToSuite' map.
    // If new default suites are needed they are allocated with 'new' and added to 'm_suites' and
    // 'm_fileToSuite' as well.
    void addExecution(const TestTag *tag);

    // The test suites discovered during TestRunner construction.
    std::vector<TestSuite *> m_suites;

    // The map of declaring file and test suite.
    std::map<std::string, std::vector<TestSuite *> > m_fileToSuite;

    // Enables or disables immediatly termination on error.
    bool m_stopOnError;

public:

    // Enables or disable stop on test error.
    void setStopOnError(bool stopOnError) { m_stopOnError = stopOnError; }

public:

    // Runs all the tests.  Returns zero on success or non-zero if any test failed.
    int run(void);

    // Called when an assertion is raised during a test execution.
    void notifyAssertion(const TestAssert& ast);

private:

    // The tests that passed.
    std::vector<TestExecution *> m_pass;

    // The tests that failed.
    std::vector<TestExecution *> m_fail;

    // The tests that were not executed.
    std::vector<TestExecution *> m_notExecuted;
    
    // Updates the user interface display before the test given by 'exec' is performed.
    void renderBeforeTest(const TestExecution& exec);

    // Renders an assertion failure.
    void renderAssertion(const TestAssert& ast);

    // Updates the user interface display after the test given by 'exec' has completed.
    void renderAfterTest(const TestExecution& exec);

    // Updates the user interface display after all tests complete.
    void renderAfterRunner(void);

protected:

    // Called before the passed test execution is performed.
    virtual void beforeTest(const TestExecution& exec) { };

    // Called after the passed test execution is performed.
    virtual void afterTest(const TestExecution& exec) { };

};




#endif
//=============================== End of File ==================================
