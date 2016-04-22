#ifndef SNAPSHOTVALIDATOR_H
#define SNAPSHOTVALIDATOR_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQ.h"
#include "AQSnapshot.h"

#include <deque>
#include <map>
#include <string>



//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates a class for validating the content of snapshots against the 
// output of the consumer.
class SnapshotValidator
{
public:

    // Constructs a new record generator which seeds based on the thread number.
    SnapshotValidator(AQ& queue, int maxOutstandingRecords);

    // Not implemented - the validator cannot be copied or assigned.
    SnapshotValidator(const SnapshotValidator& other);
    SnapshotValidator& operator=(const SnapshotValidator& other);

    // Destroys this record generator.
    virtual ~SnapshotValidator(void);

    // Adds a record to this snapshot validator.
    void add(const std::string &tag, const AQItem *rec);

    // Purges records from the list that can no longer be valid.
    void purge(void);

    // Validates the passed snapshot.
    void validate(const AQSnapshot &snap);

private:

    // Used to track record ordering rules.
    struct RecordOrder
    {
        // The first sequence number.
        unsigned long long first;

        // The last sequential record found.
        unsigned long long lastSequential;

        // The last record found.
        unsigned long long last;

    };

    // Validates the passed record matches something returned by the
    // consumer.
    void validateRecordContent(const AQSnapshot& snap, size_t i,
                               std::map<int, RecordOrder> &orderByThread);

    // Dumps a snapshot to an output stream.
    void dumpSnapshot(std::ostream& ss, const AQSnapshot& snap, size_t hightlighIdx = 0xFFFFFFFE);

    // The queue we are validating against.
    AQ& m_queue;

    // The maximum number of records held outstanding by any one producer.
    int m_maxOutstandingRecords;

    // Defines a record for comparison/reference.
    struct ReferenceRecord
    {
        // The tag for the record.
        std::string m_tag;

        // The data for the record.
        std::vector<char> m_data;

    };

    // The circular list of records for snapshot comparison.
    std::deque<ReferenceRecord *> m_records;

    // The map of record tag to record data.
    std::map<std::string, ReferenceRecord *> m_recordTagMap;

};



#endif
//=============================== End of File ==================================
