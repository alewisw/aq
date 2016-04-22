#ifndef TESTEXECUTION_H
#define TESTEXECUTION_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "TestAssert.h"
#include "TestTag.h"

#include <vector>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// Returns true if the curent test is passing, false if it is failing.
#define TEST_IS_PASSING()               TestExecution::isCurrentTestPassing()

// Returns true if the current test is failing.
#define TEST_IS_FAILING()               (!TestExecution::isCurrentTestPassing())




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

class TestSuite;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates the execution of a single test and its results.
class TestExecution
{
public:

    // Constructs a new test execution based on the passed tag that belongs
    // to the given suite.
    TestExecution(TestSuite *suite, const TestTag *tag);

    // Creates an exact copy of this test execution.
    TestExecution(const TestExecution& other);

    // Assigns the value of this test execution to exactly match another.
    TestExecution& operator=(const TestExecution& other);

    // Destroys this test execution.
    ~TestExecution(void);

private:

    // The test suite.
    TestSuite *m_suite;

    // The test tag.
    const TestTag *m_tag;

public:

    // Gets the suite for this test result.
    const TestSuite& suite(void) const { return *m_suite; }

    // The file where this test was declared.
    const char *file(void) const { return m_tag->file(); }

    // The line on which this test was declared.
    int line(void) const { return m_tag->line(); }

    // The name of this test.
    const char *name(void) const { return m_tag->name(); }

private:

    // The current test execution.
    static TestExecution *m_current;

    // Set to true if a fatal exception is being thrown.
    static bool m_throwing;

    // Set to true if this test was executed.
    bool m_executed;

    // The duration of this execution in seconds.
    double m_durationSecs;

    // The list of assertions that were raised during this test execution.
    std::vector<TestAssert> m_assertionFailure;

    // The number of assertions tested for this execution.
    size_t m_assertionCount;

public:

    // Helper class to set the current test execution.
    class Current
    {
    public:
        Current(TestExecution& exec)
        {
            TestExecution::m_current = &exec;
            TestExecution::m_throwing = false;
            exec.m_assertionFailure.clear();
            exec.m_assertionCount = 0;
        }

        // Copy and assignment is not permittted.
        Current(const Current& other);
        Current& operator=(const Current& other);

        ~Current(void)
        {
            TestExecution::m_current = NULL;
        }
    };

    // Executes this test.
    void execute(void);

    // Clears the executed flag for this test execution.
    void clearExecuted(void) { m_executed = false; m_durationSecs = 0.0; }

    // Adds the assertion failure 'ast' to the list of assertion failures
    // for the current test.
    static void addAssertion(TestAssert& ast);

    // Returns true if an exception is currently being thrown.
    static bool isThrowing(void) { return m_throwing; }

    // Marks an exception as currenty being thrown.
    static void markThrowing(void) { m_throwing = true; }

    // Returns true if the current test is passing.
    static bool isCurrentTestPassing(void);

    // Increments the counter that tracks the number of assertions tested.
    static void incrementAssertionCounter(void);

private:

    // Handles the passed assertion raised on this test execution.
    void handleAssertion(const TestAssert& ast);

public:

    // Returns the run duration of this test.
    double durationSecs(void) const { return m_durationSecs; }

    // Returns true if this test passed.
    bool isPass(void) const { return m_assertionFailure.size() == 0; }

    // Returns true if this test was executed.
    bool isExecuted(void) const { return m_executed; }

    // Returns true if this test failed.
    bool isFail(void) const { return !isPass(); }

    // The total number of assertion tests peformed in this test case.
    size_t assertionCount(void) const { return m_assertionCount; }

    // Gets the number of assertions raised in this test.
    size_t assertFailureCount(void) const { return m_assertionFailure.size(); }

    // Gets one of the assertions from this test.
    const TestAssert& assertionFailure(size_t idx) const { return m_assertionFailure[idx]; }

};



#endif
//=============================== End of File ==================================
