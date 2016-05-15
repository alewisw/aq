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
    const std::string& componentId, const std::string& tagId,
    const std::string& file)
    : m_handler(&handler)
    , m_level(level)
{
    m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID] = componentId;
    m_tierId[AQLOG_LOOKUP_TIER_TAGID] = tagId;
    m_tierId[AQLOG_LOOKUP_TIER_FILE] = file;
    if (m_tierId[2].size() > 0)
    {
        m_tierIdCount = 3;
    }
    else if (m_tierId[1].size() > 0)
    {
        m_tierIdCount = 2;
    }
    else if (m_tierId[0].size() > 0)
    {
        m_tierIdCount = 1;
    }
    else
    {
        m_tierIdCount = 0;
    }
}

//------------------------------------------------------------------------------
AQLogFilter::AQLogFilter(const AQLogFilter& other)
    : m_handler(other.m_handler)
    , m_level(other.m_level)
    , m_tierIdCount(other.m_tierIdCount)
{
    m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID] = other.m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID];
    m_tierId[AQLOG_LOOKUP_TIER_TAGID] = other.m_tierId[AQLOG_LOOKUP_TIER_TAGID];
    m_tierId[AQLOG_LOOKUP_TIER_FILE] = other.m_tierId[AQLOG_LOOKUP_TIER_FILE];
}

//------------------------------------------------------------------------------
AQLogFilter& AQLogFilter::operator=(const AQLogFilter& other)
{
    if (this != &other)
    {
        m_handler = other.m_handler;
        m_level = other.m_level;
        m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID] = other.m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID];
        m_tierId[AQLOG_LOOKUP_TIER_TAGID] = other.m_tierId[AQLOG_LOOKUP_TIER_TAGID];
        m_tierId[AQLOG_LOOKUP_TIER_FILE] = other.m_tierId[AQLOG_LOOKUP_TIER_FILE];
        m_tierIdCount = other.m_tierIdCount;

    }
    return *this;
}

//------------------------------------------------------------------------------
AQLogFilter::~AQLogFilter(void)
{
}



//=============================== End of File ==================================
