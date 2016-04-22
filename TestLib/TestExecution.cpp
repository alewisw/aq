//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "TestExecution.h"

#include "TestRunner.h"
#include "TestSuite.h"

#include "Stopwatch.h"

#include <stdexcept>

using namespace std;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

// The currently executing test.
TestExecution *TestExecution::m_current;

// Set to true when an exception is being thrown.
bool TestExecution::m_throwing;




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TestExecution::TestExecution(TestSuite *suite, const TestTag *tag)
    : m_suite(suite)
    , m_tag(tag)
    , m_executed(false)
    , m_durationSecs(0.0)
    , m_assertionCount(0)
{
}

//------------------------------------------------------------------------------
TestExecution::TestExecution(const TestExecution& other)
    : m_suite(other.m_suite)
    , m_tag(other.m_tag)
    , m_executed(other.m_executed)
    , m_durationSecs(other.m_durationSecs)
    , m_assertionFailure(other.m_assertionFailure)
    , m_assertionCount(other.m_assertionCount)
{

}

//------------------------------------------------------------------------------
TestExecution& TestExecution::operator=(const TestExecution& other)
{
    if (this != &other)
    {
        m_suite = other.m_suite;
        m_tag = other.m_tag;
        m_executed = other.m_executed;
        m_durationSecs = other.m_durationSecs;
        m_assertionFailure = other.m_assertionFailure;
        m_assertionCount = other.m_assertionCount;
    }
    return *this;
}

//------------------------------------------------------------------------------
TestExecution::~TestExecution(void)
{
}

//------------------------------------------------------------------------------
void TestExecution::execute(void)
{
    Stopwatch sw;

    m_executed = true;
    try
    {
        TestTag::Execute_fn executeFn = m_tag->executeFunction();
        executeFn(*this);
    }
    catch (const TestAssert&)
    {
        // Take no action, the assertion must already have been logged.
    }
    catch (const exception& ex)
    {
        // Unhandled standard C++ exception - log this in a new test assertion.
        TEST_ASSERT_FAILED(UNEXPECTED_EXCEPTION, false, string("std::exception: ") + ex.what());
    }
    catch (...)
    {
        // Unhandled exception of another type - log this in a new test assertion.
        TEST_ASSERT_FAILED(UNEXPECTED_EXCEPTION, false, "Unidentified exception");
    }

    // Stop any exceptions from being thrown.
    markThrowing();
    m_durationSecs = sw.elapsedSecs();
}

//------------------------------------------------------------------------------
void TestExecution::addAssertion(TestAssert& ast)
{
    ast.setExecution(m_current);
    if (m_current)
    {
        m_current->handleAssertion(ast);
    }
}

//------------------------------------------------------------------------------
bool TestExecution::isCurrentTestPassing(void)
{
    if (m_current)
    {
        return m_current->isPass();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
void TestExecution::incrementAssertionCounter(void)
{
    if (m_current)
    {
        m_current->m_assertionCount++;
    }
}

//------------------------------------------------------------------------------
void TestExecution::handleAssertion(const TestAssert& ast)
{
    m_assertionFailure.push_back(ast);

    m_suite->runner().notifyAssertion(ast);
}



//=============================== End of File ==================================
