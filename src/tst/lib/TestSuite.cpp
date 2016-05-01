//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "TestSuite.h"

#include "TestTag.h"

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

// Defines the first entry in the test suite tag list.  We rely on this being 0
// at startup (before object construction) due to BSS initialisation rules.
const TestSuite::Tag *TestSuite::Tag::m_first;


//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TestSuite::Tag::Tag(const char *file, int line, const char *suiteName, int priority)
    : m_file(file)
    , m_line(line)
    , m_suiteName(suiteName)
    , m_priority(priority)
    , m_next(m_first)
{
    m_first = this;
}

//------------------------------------------------------------------------------
TestSuite::Tag::~Tag(void)
{
}

//------------------------------------------------------------------------------
TestSuite::TestSuite(TestRunner& runner, const TestSuite::Tag& tag, 
                     TestSuite::Tag *deleteTag)
    : m_runner(runner)
    , m_tag(tag)
    , m_deleteTag(deleteTag)
{

}

//------------------------------------------------------------------------------
TestSuite::~TestSuite(void)
{
    if (m_deleteTag != NULL)
    {
        delete m_deleteTag;
    }
}

//------------------------------------------------------------------------------
void TestSuite::addExecution(const TestTag *tag)
{
    // Executors are sorted by line number of declaration.
    vector<TestExecution>::iterator it;
    for (it = m_executions.begin(); it != m_executions.end(); ++it)
    {
        if (tag->line() < it->line())
        {
            m_executions.insert(it, TestExecution(this, tag));
            return;
        }
    }
    m_executions.push_back(TestExecution(this, tag));
}




//=============================== End of File ==================================
