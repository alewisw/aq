//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "TraceManager.h"

#include "AQ.h"

#include "CtrlOverlay.h"

#include <iomanip>

using namespace std;

namespace aq {





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
TraceManager::TraceManager(LogContentMode logContent, size_t bufferSize)
    : m_logContent(logContent)
    , m_bufferSize(bufferSize)
    , m_order(0)
{
}

//------------------------------------------------------------------------------
TraceManager::~TraceManager(void)
{
    for (size_t i = 0; i < m_buffers.size(); ++i)
    {
        delete m_buffers[i];
    }
    m_buffers.clear();
}

//------------------------------------------------------------------------------
void TraceManager::clear(void)
{
    for (size_t i = 0; i < m_buffers.size(); ++i)
    {
        m_buffers[i]->clear();
    }
}

//------------------------------------------------------------------------------
TraceBuffer *TraceManager::createBuffer(const string &id, size_t recordCapacity)
{
    TraceBuffer *tb = new TraceBuffer(*this, id, recordCapacity);
    m_buffers.push_back(tb);
    return tb;
}

//------------------------------------------------------------------------------
void TraceManager::write(ostream& os) const
{
    // Start all buffers at index '0'.
    vector<size_t> index;
    for (size_t i = 0; i < m_buffers.size(); ++i)
    {
        index.push_back(0);
    }

    // Now walk through the order list finding the lowest order number and print
    // out that entry.
    for (;;)
    {
        uint64_t order = 0;
        size_t nextBuffer = m_buffers.size();

        for (size_t i = 0; i < m_buffers.size(); ++i)
        {
            if (index[i] < m_buffers[i]->recordCount())
            {
                const TraceBuffer::Record& rec = m_buffers[i]->record(index[i]);

                if (nextBuffer == m_buffers.size() || rec.order < order)
                {
                    order = rec.order;
                    nextBuffer = i;
                }
            }
        }

        if (nextBuffer == m_buffers.size())
        {
            // End of list.
            break;
        }

        // Extract the record
        const TraceBuffer::Record& rec = m_buffers[nextBuffer]->record(index[nextBuffer]);
        index[nextBuffer]++;

        // Log the record.
        m_buffers[nextBuffer]->writeRecord(os, rec);
    }
}




}
//=============================== End of File ==================================
