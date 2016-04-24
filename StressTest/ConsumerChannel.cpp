//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "ConsumerChannel.h"

#include "Main.h"
#include "Producer.h"

#include "TraceBuffer.h"

#include <iomanip>

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
ConsumerChannel::ConsumerChannel(AQReader& reader,
    const ItemGenerator& producerGen, bool checkLinkId,
    unsigned int maxOutstanding, TraceBuffer *trace)
    : m_trace(trace)
    , m_reader(reader)
    , m_producerGen(producerGen)
    , m_checkLinkId(checkLinkId)
    , m_maxOutstanding(maxOutstanding)
    , m_consumerGen(producerGen)
    , m_prng((unsigned int)(&producerGen))
{
}

//------------------------------------------------------------------------------
ConsumerChannel::~ConsumerChannel(void)
{
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        if (m_items[i].item != NULL)
        {
            freeReaderItem(m_items[i].item);
        }
    }
    m_items.clear();
}

//------------------------------------------------------------------------------
void ConsumerChannel::assertComplete(void)
{
    if (m_consumerGen.count() != m_producerGen.count())
    {
        ostringstream ss;
        ss << endl << endl << "### LOST_RECORD producer at " << m_producerGen.count() << endl
                   << endl << "                consumer at " << m_consumerGen.count() << endl;
        assertFailed(ss.str());
    }
}

//------------------------------------------------------------------------------
void ConsumerChannel::process(AQItem *item, unsigned int recLen, unsigned long long count)
{
    // Verify that we have not gone backwards.
    unsigned long long minCount = m_consumerGen.count();
    unsigned long long maxCount = minCount + m_maxOutstanding;
    if (count < minCount)
    {
        ostringstream ss;
        ss << endl << endl << "### DUPLICATE_RECORD_COUNT " << count << " when at " << m_consumerGen.count() << endl
                           << "                           found = \"" << itemToString(item) << "\"" << endl;
        dumpRecordQueue(ss);
        assertFailed(ss.str());
    }

    // Check the maximum permissible outstanding.
    if (count > maxCount)
    {
        ostringstream ss;
        ss << endl << endl << "### MISSING_RECORD " << count << " when in range [" << minCount << ", " << maxCount << "]" << endl
                           << "                   found = \"" << itemToString(item) << "\"" << endl;
        dumpRecordQueue(ss);
        assertFailed(ss.str());
    }

    size_t offset = (size_t)(count - minCount);
    if (offset < m_items.size() && m_items[offset].item != NULL)
    { 
        ostringstream ss;
        ss << endl << endl << "### DUPLICATE_RECORD " << count << " already found" << endl
                           << "                     found    = \"" << itemToString(item) << "\"" << endl
                           << "                     previous = \"" << itemToString(m_items[offset].item) << "\"" << endl;
        dumpRecordQueue(ss);
        assertFailed(ss.str());
    }

    // Put it into the vector; first reserve enough space then assign
    // the appropriate index.
    size_t i = m_items.size();
    if (i < offset + 1)
    {
        m_items.resize(offset + 1);
        for (; i < m_items.size(); ++i)
        {
            m_items[i].item = NULL;
        }
    }
    m_items[offset].item = item;
    m_items[offset].recLen = recLen;

    // Now start processing from the front of the vector and find all items we
    // can release.
    for (i = 0; i < m_items.size() && m_items[i].item != NULL; ++i)
    {
        recLen = m_items[i].recLen;

        size_t cmpSize = 0;
        uint32_t linkId = 0;
        const unsigned char *cmp = (const unsigned char *)m_consumerGen.next(cmpSize, linkId);

        if (m_checkLinkId && linkId != m_items[i].item->linkIdentifier())
        {
            ostringstream ss;
            ss << endl << endl << "### INVALID_LINK_ID found    " << m_items[i].item->linkIdentifier()
                       << endl << "                    expected " << linkId << endl;
            dumpRecordQueue(ss);
            assertFailed(ss.str());
        }

        size_t itemPos = 0;
        size_t pos = 0;
        if (recLen > cmpSize || (recLen < cmpSize && !m_reader.isExtendable()))
        {
            ostringstream ss;
            ss << endl << endl << "### INVALID_RECORD_LENGTH found    " << setw(4) << recLen  << " = \"" << itemToString(m_items[i].item) << "\""
                       << endl << "                          expected " << setw(4) << cmpSize << " = \"" << (const char *)cmp << "\"" << endl;
            dumpRecordQueue(ss);
            assertFailed(ss.str());
        }
        else if (cmpSize != recLen)
        {
            cmpSize = recLen;
            pos = RECORD_LEN_CHARS;
            itemPos = RECORD_LEN_CHARS;
        }

        for (const AQItem *it = m_items[i].item; it != NULL; it = it->next())
        {
            size_t itSize = it->size() - itemPos;
            if (pos + itSize > cmpSize || memcmp(&cmp[pos], &(*it)[itemPos], itSize) != 0)
            {
                break;
            }
            pos += itSize;
            itemPos = 0;
        }
        if (pos != cmpSize)
        {
            ostringstream ss;
            ss << endl << endl << "### INVALID_RECORD found    " << setw(4) << recLen  << " = \"" << itemToString(m_items[i].item) << "\""
                       << endl << "                   expected " << setw(4) << cmpSize << " = \"" << (const char *)cmp << "\"" << endl;
            dumpRecordQueue(ss);
            assertFailed(ss.str());
        }
    }
    if (i > 0)
    {
        // We can release the first 'i' items from the queue.  Shuffle the
        // queue and then release them.
        shuffleRecords(i);
        for (size_t j = 0; j < i; ++j)
        {
            m_reader.release(*m_items[j].item);
            freeReaderItem(m_items[j].item);
        }
        m_items.erase(m_items.begin(), m_items.begin() + i);
    }
}

//------------------------------------------------------------------------------
void ConsumerChannel::shuffleRecords(size_t count)
{
    for (size_t i = count - 1; i > 0; i--)
    {
        size_t index = m_prng.next() % (i + 1);
        Record temp = m_items[index];
        m_items[index] = m_items[i];
        m_items[i] = temp;
    }
}

//------------------------------------------------------------------------------
void ConsumerChannel::dumpRecordQueue(ostringstream& ss) const
{
    ss << "    Current Pending Items: (" << m_items.size() << ")" << endl;
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        ss << "     (" << setw(2) << i << ") ";
        if (m_items[i].item == NULL)
        {
            ss << "<empty>";
        }
        else
        {
            ss << "L=" << setw(4) << m_items[i].recLen << " \"" << itemToString(m_items[i].item) << "\"";
        }
        ss << endl;
    }
}



//=============================== End of File ==================================
