//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "TestRunner.h"

#include "Optarg.h"
#include "TestAssert.h"
#include "TestJUnitXmlReport.h"
#include "TestTag.h"
#include "TestSuite.h"

#include <string.h>
#include <iostream>
#include <fstream>

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



//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TestRunner::TestRunner(int argc, char* argv[])
    : m_stopOnError(true)
{
    parseArguments(argc, argv);

    // Get all the suites.
    map<string, vector<TestSuite *> > fileToSuite;
    for (const TestSuite::Tag *suiteTag = TestSuite::Tag::first(); suiteTag != NULL; suiteTag = suiteTag->next())
    {
        addSuite(new TestSuite(*this, *suiteTag));
    }

    // Now add all the executors to the appropriate suites.
    vector<TestSuite *> defaultSuites;
    for (const TestTag *tag = TestTag::first(); tag != NULL; tag = tag->next())
    {
        addExecution(tag);
    }
}

//------------------------------------------------------------------------------
TestRunner::~TestRunner(void)
{
    // Delete the default suites.
    for (size_t i = 0; i < m_suites.size(); ++i)
    {
        delete m_suites[i];
    }
}

//------------------------------------------------------------------------------
void TestRunner::parseArguments(int argc, char* argv[])
{
    Optarg cfg(argc, argv);

    cfg.opt('c', m_stopOnError, "Continue even when a test fails (stops on error by default).");
    cfg.opt('j', m_junitXmlFile, "The name of the JUnit XML file to write with the test results.");
}

//------------------------------------------------------------------------------
void TestRunner::addSuite(TestSuite *suite)
{
    // Order based on priority, name, file, the line number - lowest first.
    vector<TestSuite *>::iterator vit;
    bool inserted = false;
    for (vit = m_suites.begin(); vit != m_suites.end(); vit++)
    {
        int cmp = (*vit)->priority() - suite->priority();
        if (cmp == 0)
        {
            cmp = strcmp(suite->name(), (*vit)->name());
            if (cmp == 0)
            {
                cmp = strcmp(suite->file(), (*vit)->file());
                if (cmp == 0)
                {
                    cmp = suite->line() - (*vit)->line();
                }
            }
        }
        if (cmp < 0)
        {
            m_suites.insert(vit, suite);
            inserted = true;
            break;
        }
    }
    if (!inserted)
    {
        m_suites.push_back(suite);
    }

    // Insert into the map - could be multiple suites per file.
    map<string, vector<TestSuite *> >::iterator it = m_fileToSuite.find(suite->file());
    if (it == m_fileToSuite.end())
    {
        vector<TestSuite *> newVector;
        newVector.push_back(suite);
        m_fileToSuite[suite->file()] = newVector;
    }
    else
    {
        // Order based on line number, lowest first.
        for (vit = it->second.begin(); vit != it->second.end(); vit++)
        {
            if (suite->line() < (*vit)->line())
            {
                it->second.insert(vit, suite);
                return;
            }
        }
        it->second.push_back(suite);
    }
}

//------------------------------------------------------------------------------
void TestRunner::addExecution(const TestTag *tag)
{
    // Find the suite for the executor.
    TestSuite *suite;
    map<string, vector<TestSuite *> >::iterator it = m_fileToSuite.find(tag->file());
    if (it == m_fileToSuite.end())
    {
        // Create a new suite based on this executor.
        TestSuite::Tag *suiteTag = new TestSuite::Tag(tag->file(), tag->line(), "Default", 0);
        suite = new TestSuite(*this, *suiteTag, suiteTag);
        addSuite(suite);
    }
    else
    {
        // Search the list - look for the last item where the line() of the
        // executor is higher than the line() of the suite.
        suite = it->second[0];
        for (size_t i = it->second.size() - 1; i >= 1; --i)
        {
            if (tag->line() >= it->second[i]->line())
            {
                suite = it->second[i];
                break;
            }
        }
    }

    // Add the executor to the suite.
    suite->addExecution(tag);
}

//------------------------------------------------------------------------------
int TestRunner::run(void)
{
    m_pass.clear();
    m_fail.clear();
    m_notExecuted.clear();

    for (size_t i = 0; i < m_suites.size(); ++i)
    {
        TestSuite *suite = m_suites[i];

        for (size_t j = 0; j < suite->executionCount(); ++j)
        {
            TestExecution& exec = suite->execution(j);

            if (m_stopOnError && m_fail.size() > 0)
            {
                exec.clearExecuted();
                m_notExecuted.push_back(&exec);
            }
            else
            {
                // Run the test with the 'current' indicator set.
                {
                    TestExecution::Current current(exec);

                    renderBeforeTest(exec);
                    beforeTest(exec);

                    exec.execute();

                    afterTest(exec);
                    renderAfterTest(exec);
                }

                if (exec.isFail())
                {
                    m_fail.push_back(&exec);
                }
                else
                {
                    m_pass.push_back(&exec);
                }
            }
        }
    }

    renderAfterRunner();

    if (m_junitXmlFile.size() > 0)
    {
        TestJUnitXmlReport report(*this);

        ofstream os(m_junitXmlFile.c_str(), ofstream::out);
        report.publish(os);
        os.close();
    }

    return m_fail.size() == 0 ? 0 : 1;
}

//------------------------------------------------------------------------------
void TestRunner::notifyAssertion(const TestAssert& ast)
{
    renderAssertion(ast);
}

//------------------------------------------------------------------------------
void TestRunner::renderBeforeTest(const TestExecution& exec)
{
    cout << "----->> START: " << exec.suite().name() << "::" << exec.name() << endl;
}

//------------------------------------------------------------------------------
void TestRunner::renderAssertion(const TestAssert& ast)
{
    cout << "!!!!!!! ";

    switch (ast.type())
    {
    default:
        cout << "ASSERTION FAILURE";
        break;

    case TestAssert::EXPECTED_EXCEPTION:
        cout << "EXPECTED EXCEPTION NOT RAISED";
        break;

    case TestAssert::UNEXPECTED_EXCEPTION:
        cout << "UNEXPECTED EXCEPTION";
        break;
    }

    cout << endl;
    cout << "        Test:       " << ast.execution().suite().name() << "::" << ast.execution().name() << endl;
    cout << "                      declared at " << ast.execution().file() << ":" << ast.execution().line() << ")" << endl;
    if (ast.type() != TestAssert::UNEXPECTED_EXCEPTION)
    {
        cout << "        Function:   " << ast.function() << endl;
        cout << "        Location:   " << ast.file() << ":" << ast.line() << endl;
    }

    switch (ast.type())
    {
    default:
        cout << "        Expression: " << ast.expr() << endl;
        if (ast.exprDecomp().size() > 0)
        {
            cout << "        Evalulated: " << ast.exprDecomp() << endl;
        }
        break;

    case TestAssert::EXPECTED_EXCEPTION:
        cout << "        Exception:  " << ast.exceptionName() << endl;
        cout << "        Expression: " << ast.expr() << endl;
        break;

    case TestAssert::UNEXPECTED_EXCEPTION:
        cout << "        Exception:  " << ast.exceptionName() << endl;
        if (ast.exceptionMessage().size() > 0)
        {
            cout << "        Message:    " << ast.exceptionMessage() << endl;
        }
        break;
    }
}

//------------------------------------------------------------------------------
void TestRunner::renderAfterTest(const TestExecution& exec)
{
    cout << "<<----- " << (exec.isPass() ? "PASS" : "FAIL") << " : " 
        << exec.suite().name() << "::" << exec.name() << endl << endl;
}

//------------------------------------------------------------------------------
void TestRunner::renderAfterRunner(void)
{
    size_t assertionTotal = 0;
    for (size_t i = 0; i < m_pass.size(); ++i)
    {
        assertionTotal += m_pass[i]->assertionCount();
    }
    for (size_t i = 0; i < m_fail.size(); ++i)
    {
        assertionTotal += m_fail[i]->assertionCount();
    }

    size_t total = m_pass.size() + m_fail.size() + m_notExecuted.size();
    cout << "Ran " << total << (total == 1 ? " test" : " tests");
    cout << " consisting of " << assertionTotal << (assertionTotal == 1 ? " assertion" : " assertions");
    cout << ": ";
    if (m_fail.size() == 0 && m_notExecuted.size() == 0)
    {
        cout << " All passed.";
    }
    else
    {
        cout << m_pass.size() << " passed, " << m_fail.size() << " failed";
        if (m_notExecuted.size() > 0)
        {
            cout << ", and " << m_notExecuted.size() << " not executed";
        }
        cout << ".";
    }
    cout << endl;
}



//=============================== End of File ==================================
