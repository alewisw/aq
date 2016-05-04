//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogFilter.h"

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
AQLogFilter::AQLogFilter(AQLogHandler& handler, AQLogLevel_t level, 
    const std::string& key1, const std::string& key2, const std::string& key3)
    : m_handler(&handler)
    , m_level(level)
{
    m_keys[0] = key1;
    m_keys[1] = key2;
    m_keys[2] = key3;
    if (key3.size() > 0)
    {
        m_keyCount = 3;
    }
    else if (key2.size() > 0)
    {
        m_keyCount = 2;
    }
    else if (key1.size() > 0)
    {
        m_keyCount = 1;
    }
    else
    {
        m_keyCount = 0;
    }
}

//------------------------------------------------------------------------------
AQLogFilter::AQLogFilter(const AQLogFilter& other)
    : m_handler(other.m_handler)
    , m_level(other.m_level)
    , m_keyCount(other.m_keyCount)
{
    m_keys[0] = other.m_keys[0];
    m_keys[1] = other.m_keys[1];
    m_keys[2] = other.m_keys[2];
}

//------------------------------------------------------------------------------
AQLogFilter& AQLogFilter::operator=(const AQLogFilter& other)
{
    if (this != &other)
    {
        m_handler = other.m_handler;
        m_level = other.m_level;
        m_keys[0] = other.m_keys[0];
        m_keys[1] = other.m_keys[1];
        m_keys[2] = other.m_keys[2];
        m_keyCount = other.m_keyCount;

    }
    return *this;
}

//------------------------------------------------------------------------------
AQLogFilter::~AQLogFilter(void)
{
}



//=============================== End of File ==================================
