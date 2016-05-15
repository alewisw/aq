//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "LogReader.h"

#include "Timer.h"

#include <stdexcept>

using namespace aqosa;
using namespace std;

namespace aqlog
{




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
LogReader::LogReader(IAQSharedMemory& sm)
    : m_aq(sm)
{
    m_aq.format(8, 1000, AQ::OPTION_EXTENDABLE);
}

//------------------------------------------------------------------------------
LogReader::~LogReader(void)
{
    for (list<AQLogRecord *>::iterator it = m_pending.begin(); it != m_pending.end(); ++it)
    {
        delete *it;
    }
    for (set<AQLogRecord *>::iterator it = m_outstanding.begin(); it != m_outstanding.end(); ++it)
    {
        delete *it;
    }
    for (deque<AQLogRecord *>::iterator it = m_free.begin(); it != m_free.end(); ++it)
    {
        delete *it;
    }
}

//------------------------------------------------------------------------------
AQLogRecord *LogReader::retrieve(uint32_t& maxRecallMs)
{
    while (m_pending.size() < PENDING_WINDOW_SIZE)
    {
        AQLogRecord *rec = alloc();

        if (!m_aq.retrieve(rec->aqItem()))
        {
            // No records available.
            free(rec);
            break;
        }
        else
        {
            AQLogRecord::PopulateOutcome outcome = rec->populate();
            if (outcome != AQLogRecord::POPULATE_SUCCESS)
            {
                m_aq.release(rec->aqItem());
                free(rec);
            }
            else
            {
                // Record is available.  Put it into the pending queue with the highest
                // timestamp appearing first.
                list<AQLogRecord *>::iterator it = m_pending.begin();
                while (it != m_pending.end()
                    && rec->timestampNs() < (*it)->timestampNs()
                    && Timer::elapsed((*it)->processTimeMs()) < PENDING_MAXIMUM_WINDOW_MS)
                {
                    it++;
                }
                m_pending.insert(it, rec);
            }
        }
    }

    // Now look at the last item in the pending list.
    AQLogRecord *rec = NULL;
    if (m_pending.size() > 0)
    {
        rec = m_pending.back();
        if (   m_pending.size() == PENDING_WINDOW_SIZE 
            || Timer::elapsed(rec->processTimeMs()) > PENDING_MINIMUM_WINDOW_MS)
        {
            m_pending.pop_back();
            m_outstanding.insert(rec);
        }
        else
        {
            rec = NULL;
        }
    }

    // Finally calculate the recall time.
    if (m_pending.size() > 0)
    {
        maxRecallMs = Timer::elapsed(m_pending.back()->processTimeMs());
        if (maxRecallMs >= PENDING_MINIMUM_WINDOW_MS)
        {
            maxRecallMs = 0;
        }
        else
        {
            maxRecallMs = PENDING_MINIMUM_WINDOW_MS - maxRecallMs;
        }
    }
    else
    {
        maxRecallMs = 1000;
    }

    return rec;
}

//------------------------------------------------------------------------------
void LogReader::release(AQLogRecord *rec)
{
    set<AQLogRecord *>::iterator it = m_outstanding.find(rec);
    if (it == m_outstanding.end())
    {
        throw invalid_argument("AQLogRecord is not currently outstanding");
    }
    m_outstanding.erase(it);
    m_aq.release(rec->aqItem());

    free(rec);
}

//------------------------------------------------------------------------------
AQLogRecord *LogReader::alloc(void)
{
    AQLogRecord *rec;
    if (m_free.size() == 0)
    {
        rec = new AQLogRecord;
    }
    else
    {
        rec = m_free.front();
        m_free.pop_front();
    }
    return rec;
}

//------------------------------------------------------------------------------
void LogReader::free(AQLogRecord *rec)
{
    m_free.push_back(rec);
}



}
//=============================== End of File ==================================
