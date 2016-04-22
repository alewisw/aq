//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "TestJUnitXmlReport.h"
#include "TestExecution.h"
#include "TestRunner.h"
#include "TestSuite.h"

#include <iomanip>
#include <sstream>

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
TestJUnitXmlReport::TestJUnitXmlReport(TestRunner& runner)
    : m_runner(runner)
{
}

//------------------------------------------------------------------------------
TestJUnitXmlReport::~TestJUnitXmlReport(void)
{
}

//------------------------------------------------------------------------------
void TestJUnitXmlReport::publish(ostream& os)
{
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << endl;
    os << "<testsuites>" << endl;
    for (size_t i = 0; i < m_runner.suiteCount(); ++i)
    {
        publishSuite(os, m_runner.suite(i));
    }
    os << "</testsuites>" << endl;
}

//------------------------------------------------------------------------------
void TestJUnitXmlReport::publishSuite(std::ostream& os, const TestSuite& suite)
{
    size_t skipped = 0;
    size_t failures = 0;
    for (size_t i = 0; i < suite.executionCount(); ++i)
    {
        const TestExecution& exec = suite.execution(i);

        if (!exec.isExecuted())
        {
            skipped++;
        }
        else if (exec.isFail())
        {
            failures++;
        }
    }

    os << "    <testsuite"
        << encodeAttribute("name", suite.name())
        << encodeAttribute("errors", "0")
        << encodeAttribute("tests", suite.executionCount())
        << encodeAttribute("skipped", skipped)
        << encodeAttribute("failures", failures)
        << ">" << endl;

    for (size_t i = 0; i < suite.executionCount(); ++i)
    {
        publishExecution(os, suite.execution(i));
    }

    os << "    </testsuite>" << endl;
}

//------------------------------------------------------------------------------
void TestJUnitXmlReport::publishExecution(std::ostream& os, const TestExecution& exec)
{
    os << "        <testcase"
        << encodeAttribute("classname", exec.suite().name())
        << encodeAttribute("name", exec.name())
        << encodeAttribute("time", exec.durationSecs())
        << ">" << endl;

    // Skipped tests have no additional information.
    if (!exec.isExecuted())
    {
        os << "           <skipped />" << endl;
        os << "        </testcase>" << endl;
        return;
    }

    // If the test failed, show all the failure messages.
    if (exec.isFail())
    {
        const TestAssert& ast = exec.assertionFailure(exec.assertFailureCount() - 1);

        const char *tagStr;
        const char *typeStr;
        if (ast.type() == TestAssert::UNEXPECTED_EXCEPTION)
        {
            tagStr = "error";
            typeStr = "UNEXPECTED_EXCEPTION";
        }
        else
        {
            tagStr = "failure";
            if (ast.type() == TestAssert::EXPECTED_EXCEPTION)
            {
                typeStr = "EXPECTED_EXCEPTION";
            }
            else
            {
                typeStr = "ASSERT";
            }
        }

        os << "           <" << tagStr
            << encodeAttribute("type", typeStr)
            << encodeAttribute("message", ast.expr())
            << " />" << endl;
    }

    os << "        </testcase>" << endl;
}

//------------------------------------------------------------------------------
string TestJUnitXmlReport::encodeAttribute(const char *name, const std::string& value)
{
    string prefix(" ");
    prefix += name;
    prefix += "=\"";

    return encodeText(value, prefix.c_str(), "\"");
}

//------------------------------------------------------------------------------
string TestJUnitXmlReport::encodeAttribute(const char *name, size_t value)
{
    ostringstream ss;

    ss << " " << name << "=\"" << value << "\"";

    return ss.str();
}

//------------------------------------------------------------------------------
string TestJUnitXmlReport::encodeAttribute(const char *name, double value)
{
    ostringstream ss;

    ss << " " << name << "=\"" << fixed << setprecision(3) << value << "\"";

    return ss.str();
}

//------------------------------------------------------------------------------
string TestJUnitXmlReport::encodeAttribute(const char *name, time_t value)
{
    return encodeAttribute(name, "2013-05-24T10:23:58");
}

//------------------------------------------------------------------------------
string TestJUnitXmlReport::encodeText(const string& test, const char *prefix, const char *suffix)
{
    ostringstream ss;

    ss << prefix;
    for (size_t i = 0; i < test.size(); ++i)
    {
        switch (test[i])
        {
        case '"':   ss << "&quot;"; break;
        case '\'':  ss << "&apos;"; break;
        case '<':   ss << "&lt;";   break;
        case '>':   ss << "&gt;";   break;
        case '&':   ss << "&amp;";  break;
        default:
            ss << test[i];
            break;
        }
    }
    ss << suffix;

    return ss.str();
}




//=============================== End of File ==================================
