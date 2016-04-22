#ifndef TESTASSERT_H
#define TESTASSERT_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <string>



//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// The two main assertion macros - CHECK() does not fail the test immediatly,
// however it will fail at the end.  REQUIRE() causes an immediate test failure.
#define CHECK(__expr)                   TEST_ASSERT(false, __expr)
#define REQUIRE(__expr)                 TEST_ASSERT(true, __expr)

#define CHECK_EXCEPTION(__expr, __ex)   TEST_EXCEPTION(false, __expr, __ex)
#define REQUIRE_EXCEPTION(__expr, __ex) TEST_EXCEPTION(true, __expr, __ex)


// Asserts that an exception of type __exception is thrown when the expression
// __expr is executed.
#define TEST_EXCEPTION(__throwIfFalse, __expr, __exception)                     \
do                                                                              \
{                                                                               \
    try                                                                         \
    {                                                                           \
        __expr;                                                                 \
        TEST_ASSERT_FAILED(EXPECTED_EXCEPTION, __throwIfFalse,                  \
          "Exception " #__exception " must be thrown as a result of ("          \
          #__expr ")");                                                         \
    }                                                                           \
    catch (const __exception&)                                                  \
    {                                                                           \
    }                                                                           \
} while(0)


// Asserts that the passed expression '__expr' evaluates to 'true'.  If the value
// is false then:
//  - A TestAssert object is constructed and added to the current test 
//    execution.
//  - If '__throwIfFalse' is true then the TestAssert object is then thrown.
//  - 
#define TEST_ASSERT(__throwIfFalse, __expr)                                     \
do                                                                              \
{                                                                               \
    TestExecution::incrementAssertionCounter();                                 \
    if (!(__expr))                                                              \
    {                                                                           \
        TEST_ASSERT_FAILED(ASSERT, __throwIfFalse, #__expr);                    \
    }                                                                           \
} while (0)

// Used internally when an assertion fails.
#define TEST_ASSERT_FAILED(__type, __throwIfFailed, __exprStr)                  \
do                                                                              \
{                                                                               \
    TestAssert ast(__FILE__, __FUNCTION__, __LINE__, TestAssert::__type,        \
                   __exprStr);                                                  \
    TestExecution::addAssertion(ast);                                           \
    if (__throwIfFailed && !TestExecution::isThrowing())                        \
    {                                                                           \
        TestExecution::markThrowing();                                          \
        throw ast;                                                              \
    }                                                                           \
} while (0)



//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class TestExecution;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates all the information regarding an assertion failure.
class TestAssert
{
public:

    // The types of assertion failures.
    enum Type
    {
        // A normal assertion failure.
        ASSERT,

        // This was an exception that was expected but did not occur.
        EXPECTED_EXCEPTION,

        // This was an exception that was unexpected.
        UNEXPECTED_EXCEPTION,
    };

    // Constructs a new test assertion failure at the specified location.
    // The 'expr' argument is the string expression that failed or a description
    // thereof.
    TestAssert(const char *file, const char *func, int line,
        Type type, const std::string& expr);

    // Creates an exact copy of this test assertion failure.
    TestAssert(const TestAssert& other);

    // Assigns the value of this test assertion failure to exactly match another.
    TestAssert& operator=(const TestAssert& other);

    // Destroys this assertion failure report.
    ~TestAssert(void);

private:

    // The file where the assertion occurred.
    const char *m_file;

    // The function in which the assertion occurred.
    const char *m_func;

    // The line on which the assertion occurred.
    int m_line;

    // The type of exception.
    Type m_type;

    // The expression that failed.
    std::string m_expr;

    // The execution, attached to this assertion.
    const TestExecution *m_exec;

public:

    // The file where the assertion occurred.
    const char *file(void) const { return m_file; }

    // The function in which the assertion occurred.
    const char *function(void) const { return m_func; }

    // The line on which the assertion occurred.
    int line(void) const { return m_line; }

    // The type of assertion failure.
    Type type(void) const { return m_type; }

    // The expression related to the failure.
    const std::string& expr(void) const { return m_expr; }

    // The execution attached to this exception.
    const TestExecution& execution(void) const { return *m_exec; }

    // Attaches an execution to this exception.
    void setExecution(TestExecution *exec) { m_exec = exec; }

};




#endif
//=============================== End of File ==================================
