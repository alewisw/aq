#ifndef TESTTAG_H
#define TESTTAG_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// Defines a new test named 'testName' for the currently defined test suite.
//
// This is used in the following way:
//
// TEST(name)
// {
//     ... commands ...
// }
#define TEST(testName)                                                          \
static void Wrapper_##testName(TestExecution& exec);                            \
static const TestTag TestTag_##testName(__FILE__, __LINE__,                     \
                                        #testName, Wrapper_##testName);         \
static void testName(void);                                                     \
static void Wrapper_##testName(TestExecution& exec) { (void)exec; testName(); } \
static void testName(void)




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class TestExecution;
class TestSuite;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// A tag that describes a single test case.
class TestTag
{
public:

    // The type of the execution callback function.
    typedef void(*Execute_fn)(TestExecution& exec);

    // Constructs a new TestTag - do not call directly, always use the TEST()
    // macro.
    TestTag(const char *file, int line, const char *testName,
            TestTag::Execute_fn executeFn);

    // Copy and assignment not supported - not implemented.
    TestTag(const TestTag& other);
    TestTag& operator=(const TestTag& other);

    // Destroys this tag.
    ~TestTag(void);

private:

    // The file that owns this tag.
    const char *const m_file;

    // The line where the tag appears.
    const int m_line;

    // The name of the test that is executed.
    const char *const m_testName;

    // The execution function.
    const Execute_fn m_executeFn;

    // The next test tag in the singly linked list.
    const TestTag *const m_next;

    // The first test tag in the singly linked list.
    static const TestTag *m_first;

public:

    // Returns the first test tag.
    static const TestTag *first(void) { return m_first; }

    // Returns the next test tag in the singly linked list.
    const TestTag *next(void) const { return m_next; }

    // The file where this tag was declared.
    const char *file(void) const { return m_file; }

    // The line on which this tag was declared.
    int line(void) const { return m_line; }

    // The name of this tag.
    const char *name(void) const { return m_testName; }

    // The function that executes the test.
    Execute_fn executeFunction(void) const { return m_executeFn; }

};



#endif
//=============================== End of File ==================================
