//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "SnapshotValidator.h"

#include "Main.h"

#include <string.h>
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
SnapshotValidator::SnapshotValidator(AQ& queue, int maxOutstandingRecords)
    : m_queue(queue)
    , m_maxOutstandingRecords(maxOutstandingRecords)
{
}

//------------------------------------------------------------------------------
SnapshotValidator::~SnapshotValidator(void)
{
    m_recordTagMap.clear();
    while (m_records.size() > 0)
    {
        delete m_records.front();
        m_records.pop_front();
    }
}

//------------------------------------------------------------------------------
void SnapshotValidator::add(const string &tag, const AQItem *rec)
{
    ReferenceRecord *rr = new ReferenceRecord();

    rr->m_tag = tag;
    rr->m_linkId = rec->linkIdentifier();
    rr->m_queueId = rec->queueIdentifier();
    for (const AQItem *it = rec; it != NULL; it = it->next())
    {
        size_t off = rr->m_data.size();
        rr->m_data.resize(off + it->size());
        memcpy(&rr->m_data[off], &(*it)[0], it->size());
    }

    if (m_recordTagMap.find(rr->m_tag) != m_recordTagMap.end())
    {
        ostringstream ss;

        ss << "Duplicate tag \"" << rr->m_tag << "\" encountered during snapshot validator record collection";
        assertFailed(ss.str());
    }
    m_records.push_back(rr);
    m_recordTagMap[rr->m_tag] = rr;
}

//------------------------------------------------------------------------------
void SnapshotValidator::purge(void)
{
    while (m_records.size() > m_queue.pageCount())
    {
        ReferenceRecord *rr = m_records.front();
        m_records.pop_front();
        m_recordTagMap.erase(rr->m_tag);
        delete rr;
    }
}

//------------------------------------------------------------------------------
void SnapshotValidator::validate(const AQSnapshot &snap)
{
    map<int, RecordOrder> orderByThread;

    // Validate checksums.
    for (size_t i = 0; i < snap.size(); ++i)
    {
        if (snap[i].isCommitted() && !snap[i].isChecksumValid())
        {
            ostringstream ss;

            ss << "### CHECKSUM FAILED IN SNAPSHOT" << endl;
            dumpSnapshot(ss, snap);
            assertFailed(ss.str());
        }
    }
    
    // Validate the content of all the records in the snapshot.
    for (size_t i = 0; i < snap.size(); ++i)
    {
        if (snap[i].isCommitted() && snap[i].isChecksumValid())
        { 
            validateRecordContent(snap, i, orderByThread);
        }
    }

    // Validate the ordering of the records, checking for missing records.
    for (map<int, RecordOrder>::iterator it = orderByThread.begin(); it != orderByThread.end(); ++it)
    {
        const RecordOrder &ro = it->second;

        unsigned long long gap = ro.last - ro.lastSequential;
        if (gap > (unsigned long long)m_maxOutstandingRecords)
        {
            ostringstream ss;

            ss << "Producer thread ID " << it->first << " has records [" << ro.first 
               << ", " << ro.lastSequential << "] inclusive, however record " << ro.last 
               << " also exists giving a gap of " << gap 
                << " greater than the maximum outstanding of " << m_maxOutstandingRecords << endl;
            dumpSnapshot(ss, snap);
            assertFailed(ss.str());
        }
    }
}

//------------------------------------------------------------------------------
void SnapshotValidator::validateRecordContent(const AQSnapshot& snap, size_t i,
                                              map<int, RecordOrder>& orderByThread)
{
    const AQItem& rec = snap[i];

    unsigned int recLen;
    int threadId;
    unsigned long long count;
    string tag = parseRecordTag("snapshot-validate", &rec, recLen, threadId, count);

    // Update the ordering information table.
    map<int, RecordOrder>::iterator rit = orderByThread.find(threadId);
    if (rit == orderByThread.end())
    {
        RecordOrder ro;
        ro.first = count;
        ro.last = count;
        ro.lastSequential = count;
        orderByThread[threadId] = ro;
    }
    else
    {
        if (count < rit->second.last)
        { 
            ostringstream ss;

            ss << "Tag \"" << tag << "\" encountered in snapshot after a tag with thread ID "
               << threadId << " and sequence " << count << endl;
            dumpSnapshot(ss, snap, i);
            assertFailed(ss.str());
        }
        else
        {
            rit->second.last = count;
            if (rit->second.lastSequential + 1 == count)
            {
                rit->second.lastSequential = count;
            }
        }
    }

    // Check the content.
    map<string, ReferenceRecord *>::iterator it = m_recordTagMap.find(tag);
    if (it == m_recordTagMap.end())
    {
        ostringstream ss;

        ss << "Tag \"" << tag << "\" encountered in snapshot but does not exist in consumed record set" << endl;
        dumpSnapshot(ss, snap, i);
        assertFailed(ss.str());
        return;
    }

    size_t totalSize = 0;
    for (const AQItem *it = &rec; it != NULL; it = it->next())
    {
        totalSize += it->size();
    }

    ReferenceRecord *rr = it->second;
    if (rr->m_data.size() != totalSize)
    {
        ostringstream ss;

        ss << "Tag \"" << tag << "\" record size mismatch encountered in snapshot, snapshot gave " 
            << totalSize << " however consumer received " << rr->m_data.size() << endl;
        dumpSnapshot(ss, snap, i);
        assertFailed(ss.str());
    }

    size_t pos = 0;
    for (const AQItem *it = &rec; it != NULL; it = it->next())
    {
        if (memcmp(&rr->m_data[pos], &(*it)[0], it->size()) != 0)
        {
            ostringstream ss;

            ss << "Tag \"" << tag << "\" record data mismatch encountered in snapshot" << endl;
            dumpSnapshot(ss, snap, i);
            assertFailed(ss.str());
        }
        pos += it->size();
    }

    if (rr->m_queueId != rec.queueIdentifier())
    {
        ostringstream ss;

        ss << "Tag \"" << tag << "\" queue identifier mismatch encountered in snapshot" << endl;
        dumpSnapshot(ss, snap, i);
        assertFailed(ss.str());
    }
    if (rr->m_linkId != rec.linkIdentifier())
    {
        ostringstream ss;

        ss << "Tag \"" << tag << "\" link identifier mismatch encountered in snapshot" << endl;
        dumpSnapshot(ss, snap, i);
        assertFailed(ss.str());
    }
}

//------------------------------------------------------------------------------
void SnapshotValidator::dumpSnapshot(std::ostream& ss, const AQSnapshot& snap, size_t highlightIdx)
{
    for (size_t i = 0; i < snap.size(); ++i)
    {
        if (i == highlightIdx)
        {
            ss << " ***** ";
        }
        else
        {
            ss << "       ";
        }
        ss << " (" << setw(4) << (i + 1) << ") " << itemToString(&snap[i]) << endl;
    }
}




//=============================== End of File ==================================
