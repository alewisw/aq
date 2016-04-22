#ifndef TESTSUITE_H
#define TESTSUITE_H
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

#include <vector>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// Defines a test suite - all TEST() instances in this file that appear after
// this suite belong to this suite.
#define TEST_SUITE(suiteName)                                                   \
static TestSuite::Tag Suite_##suiteName(__FILE__, __LINE__, #suiteName, 0)      

// When defined in this way the the suite is placed first in the list of tests
// to execute.
#define TEST_SUITE_FIRST(suiteName)                                             \
static TestSuite::Tag Suite_##suiteName(__FILE__, __LINE__, #suiteName, 1)      




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class TestRunner;
class TestTag;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// A TestSuite consists of zero or more test cases that are executed as a group.
class TestSuite
{
public:

    // Defines a TestSuite tag - that is the descriptor for a particular test
    // suite.
    class Tag
    {
    public:
        // Constructs a new Tag - do not call directly, always use the 
        // TEST_SUITE() macro.
        Tag(const char *file, int line, const char *suiteName, int priority);
        
        // Copy and assignment not supported - not implemented.
        Tag(const Tag& other);
        Tag& operator=(const Tag& other);

        // Destroys this tag.
        ~Tag(void);

    private:

        // The file that owns this suite.
        const char *const m_file;

        // The line where the suite appears.
        const int m_line;

        // The name of the test suite.
        const char *const m_suiteName;

        // The priority of this suite - higher numbers mean it is
        // run earlier.
        const int m_priority;

        // The next test suite tag in the singly linked list.
        const Tag *const m_next;

        // The first test suite tag in the singly linked list.
        static const Tag *m_first;

    public:

        // Returns the first test suite tag.
        static const Tag *first(void) { return m_first; }

        // Returns the next test suite tag in the singly linked list.
        const Tag *next(void) const { return m_next; }

        // The file where this suite was declared.
        const char *file(void) const { return m_file; }

        // The line on which this suite was declared.
        int line(void) const { return m_line; }

        // The name of this suite.
        const char *name(void) const { return m_suiteName; }

        // The priority of this suite.
        int priority(void) const { return m_priority; }
    };

    // Constructs a new TestSuite using the passed tag.  If 'deleteTag'
    // is non-NULL it will be deleted when this TestSuite is deleted.
    TestSuite(TestRunner& runner, const Tag& tag, Tag *deleteTag = NULL);

    // Copy and assignment not supported - not implemented.
    TestSuite(const TestSuite& other);
    TestSuite& operator=(const TestSuite& other);

    // Destroys this suite.
    ~TestSuite(void);

private:

    // The owning test runner.
    TestRunner& m_runner;

    // The tag for this suite.
    const Tag& m_tag;

    // The tag to delete or NULL if there is no tag to delete.
    Tag *m_deleteTag;

public:

    // The runner that owns this suite.
    TestRunner& runner(void) { return m_runner; }

    // The file where this suite was declared.
    const char *file(void) const { return m_tag.file(); }

    // The line on which this suite was declared.
    int line(void) const { return m_tag.line(); }

    // The name of this suite.
    const char *name(void) const { return m_tag.name(); }

    // The priority of this suite.
    int priority(void) const { return m_tag.priority(); }

    // Adds an execution to this suite.
    void addExecution(const TestTag *tag);

    // Gets the number of executions in this test suite.
    size_t executionCount(void) const { return m_executions.size(); }

    // Gets one of the executions from this test suite.
    TestExecution& execution(size_t idx) { return m_executions[idx]; }
    const TestExecution& execution(size_t idx) const { return m_executions[idx]; }

private:

    // The list of executors.
    std::vector<TestExecution> m_executions;

};



#endif
//=============================== End of File ==================================
