//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogRecord.h"

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
AQLogRecord::AQLogRecord(AQLogLevel_t level, const std::string& componentId, 
    const std::string& tagId, const std::string& file, const std::string& message)
    : m_level(level)
    , m_message(message)
{
    m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID] = componentId;
    m_tierId[AQLOG_LOOKUP_TIER_TAGID] = tagId;
    m_tierId[AQLOG_LOOKUP_TIER_FILE] = file;
}

//------------------------------------------------------------------------------
AQLogRecord::AQLogRecord(const AQLogRecord& other)
    : m_level(other.m_level)
    , m_message(other.m_message)
{
    for (size_t i = 0; i < AQLOG_LOOKUP_TIER_COUNT; ++i)
    {
        m_tierId[i] = other.m_tierId[i];
    }
}

//------------------------------------------------------------------------------
AQLogRecord& AQLogRecord::operator=(const AQLogRecord& other)
{
    if (this != &other)
    {
        m_level = other.m_level;
        m_message = other.m_message;
        for (size_t i = 0; i < AQLOG_LOOKUP_TIER_COUNT; ++i)
        {
            m_tierId[i] = other.m_tierId[i];
        }
    }
    return *this;
}

//------------------------------------------------------------------------------
AQLogRecord::~AQLogRecord(void)
{
}




//=============================== End of File ==================================
