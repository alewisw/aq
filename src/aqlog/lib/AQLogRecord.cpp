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

#include "Timer.h"

using namespace aqosa;
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
AQLogRecord::AQLogRecord(void)
    : m_level(AQLOG_LEVEL_CRITICAL)
    , m_overlay(NULL)
    , m_processTimeMs(0)
{
}

//------------------------------------------------------------------------------
AQLogRecord::AQLogRecord(AQLogLevel_t level, const char *componentId,
    const char *tagId, const char *file)
    : m_level(level)
    , m_overlay(NULL)
    , m_processTimeMs(0)
{
    m_tierId[AQLOG_LOOKUP_TIER_COMPONENTID] = componentId;
    m_tierId[AQLOG_LOOKUP_TIER_TAGID] = tagId;
    m_tierId[AQLOG_LOOKUP_TIER_FILE] = file;
}

//------------------------------------------------------------------------------
AQLogRecord::~AQLogRecord(void)
{
}

//------------------------------------------------------------------------------
AQLogRecord::PopulateOutcome AQLogRecord::populate(void)
{
    m_message.clear();
    m_processTimeMs = Timer::start();

    // Check the item - make sure it is valid.
    if (!m_item.isCommitted())
    {
        return POPULATE_ERROR_UNCOMMITTED;
    }
    else if (!m_item.isChecksumValid())
    {
        return POPULATE_ERROR_CHECKSUM;
    }
    else if (m_item.size() < offsetof(Overlay, strData))
    {
        return POPULATE_ERROR_TRUNCATED_HEADER;
    }

    // Header is valid.
    m_overlay = (Overlay *)&m_item[0];
    m_level = (AQLogLevel_t)m_overlay->logLevel;

    // Check the data field.
    if (m_item.size() < offsetof(Overlay, strData) + m_overlay->dataSize)
    {
        return POPULATE_ERROR_TRUNCATED_DATA;
    }

    // Obtain all the string fields.
    const char *str = &m_overlay->strData[m_overlay->dataSize];
    size_t strLen = m_item.size() - offsetof(Overlay, strData) - m_overlay->dataSize;
    for (int i = 0; i < AQLOG_LOOKUP_TIER_COUNT + AQLOG_EXTRA_TIER_COUNT; ++i)
    {
        size_t len = 1 + strnlen(str, strLen);
        if (len > strLen)
        {
            // Out of space in the buffer.  Looks to be corrupted.
            return (PopulateOutcome)(POPULATE_ERROR_TRUNCATED_COMPONENT_ID + i);
        }
        m_tierId[i] = str;
        str += len;
        strLen -= len;
    }

    // Obtain the message.
    if (strLen > 0)
    {
        AQLogStringBuilder::iterator it = m_message.end();
        m_message.insertPointer(it, str, strLen);
        for (const AQItem *mi = m_item.next(); mi != NULL; mi = mi->next())
        {
            it = m_message.end();
            m_message.insertPointer(it, (const char *)&(*mi)[0], mi->size());
        }
    }

    return POPULATE_SUCCESS;
}




//=============================== End of File ==================================
