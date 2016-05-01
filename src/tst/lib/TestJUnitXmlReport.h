#ifndef TESTJUNITXMLREPORT_H
#define TESTJUNITXMLREPORT_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <ostream>



//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class TestExecution;
class TestRunner;
class TestSuite;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Supports the generation of JUnit XML style reports for a TestRunner 
// execution.
class TestJUnitXmlReport
{
public:

    // Constructs a new TestJUnitXmlReport object; this object does nothing
    // until 'publish' is called.
    TestJUnitXmlReport(TestRunner& runner);

    // Copy and assignment not supported - not implemented.
    TestJUnitXmlReport(const TestJUnitXmlReport& other);
    TestJUnitXmlReport& operator=(const TestJUnitXmlReport& other);

    // Destroys this report generator.
    ~TestJUnitXmlReport(void);

private:

    // The runner that will be reported on.
    TestRunner& m_runner;
    
public:

    // Publishes the report to the output stream given by 'os'.
    void publish(std::ostream& os);

private:

    // Publishes a suite report to the output stream given by 'os'.
    void publishSuite(std::ostream& os, const TestSuite& suite);

    // Publishes a test exection report to the output stream given by 'os'.
    void publishExecution(std::ostream& os, const TestExecution& exec);

    // Encodes an attribute named 'name' with value 'value' into a string.
    static std::string encodeAttribute(const char *name, const std::string& value);
    static std::string encodeAttribute(const char *name, size_t value);
    static std::string encodeAttribute(const char *name, double value);
    static std::string encodeAttribute(const char *name, time_t value);

    // Encodes text according to the XML encoding rules into a string.
    static std::string encodeText(const std::string& test, const char *prefix = "", const char *suffix = "");

};



#endif
//=============================== End of File ==================================
