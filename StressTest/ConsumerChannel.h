#ifndef CONSUMERCHANNEL_H
#define CONSUMERCHANNEL_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "ItemGenerator.h"

#include "AQReader.h"
#include "TraceManager.h"

#include <sstream>

using namespace aq;




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declaration.
class Producer;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates the logic that compares the item received for a particular 
// producer.
class ConsumerChannel
{
public:

    // Constructs a new reader channel with the given thread identifier for
    // the specified producer.
    ConsumerChannel(AQReader& reader, const ItemGenerator& producerGen,
                    unsigned int maxOutstanding, TraceBuffer *trace);

    // Destroys this thread.
    virtual ~ConsumerChannel(void);

    // Not implmented - cannot be copied or assigned.
    ConsumerChannel(const ConsumerChannel& other);
    ConsumerChannel& operator=(const ConsumerChannel& other);

    // Called from the main thread when the reader and producer should be fully
    // synchronized.
    void assertComplete(void);

    // Gives a record to this consumer channel for further processing.  The 
    // 'count' field defines the counter that was decoded from the item.
    // The 'recLen' gives the record length as defined in the header.
    //
    // The item is now owned by this consumer - it must be freed by calling
    // freeReaderItem().
    void process(AQItem *item, unsigned int recLen, unsigned long long count);

private:

    // Shuffles the records array up to index 'count' - 1.
    void shuffleRecords(size_t count);

    // Dumps the content of the record queue into the passed string stream.
    void dumpRecordQueue(std::ostringstream& ss) const;

    // The trace buffer.
    TraceBuffer *m_trace;

    // The reader for this channel.
    AQReader& m_reader;

    // The producer generator for this channel.
    const ItemGenerator& m_producerGen;

    // The maximum number of outstanding records.
    const unsigned int m_maxOutstanding;

    // The record generator used for comparison of records.
    ItemGenerator m_consumerGen;

    // The PRNG used for release ordering.
    Prng m_prng;

    // Defines a record.
    struct Record
    {
        // The actual item.
        AQItem *item;

        // The record length field from this item.
        size_t recLen;
    };

    // The buffer of pending items.  An entry with memSize of 0 is not
    // in use.
    std::vector<Record> m_items;

};



#endif
//=============================== End of File ==================================
