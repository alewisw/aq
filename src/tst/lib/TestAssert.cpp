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
string TestAssert::m_emptyString;




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TestAssert::TestAssert(const char *file, const char *func, int line, Type type, 
    const std::string& expr, const std::string& decomp)
    : m_file(file)
    , m_func(func)
    , m_line(line)
    , m_type(type)
    , m_expr(expr)
    , m_decomp(decomp)
    , m_exec(NULL)
{
}

//------------------------------------------------------------------------------
TestAssert::TestAssert(const TestAssert& other)
    : m_file(other.m_file)
    , m_func(other.m_func)
    , m_line(other.m_line)
    , m_type(other.m_type)
    , m_expr(other.m_expr)
    , m_decomp(other.m_decomp)
    , m_exec(other.m_exec)
{
}

//------------------------------------------------------------------------------
TestAssert& TestAssert::operator=(const TestAssert& other)
{
    if (this != &other)
    {
        m_file = other.m_file;
        m_func = other.m_func;
        m_line = other.m_line;
        m_type = other.m_type;
        m_expr = other.m_expr;
        m_decomp = other.m_decomp;
        m_exec = other.m_exec;
    }
    return *this;

}

//------------------------------------------------------------------------------
TestAssert::~TestAssert(void)
{
}




//=============================== End of File ==================================
