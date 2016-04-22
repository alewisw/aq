//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Main.h"

#include "AQTest.h"
#include "TestPointAction.h"




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
TEST_SUITE(UtQueueId);

//------------------------------------------------------------------------------
AQTEST(given_QueueFullyPopulatedTenTimes_when_QueueIdRead_then_AllUnique)
{
    set<unsigned int> s;
    for (int i = 0; i < 12; ++i)
    {
        aq.enqueue(aq.pageCount() - 1);
        for (size_t j = 0; j < aq.pageCount() - 1; ++j)
        {
            AQItem ritem;
            REQUIRE(aq.reader.retrieve(ritem));
            REQUIRE(s.find(ritem.queueIdentifier()) == s.end());
            s.insert(ritem.queueIdentifier());
            aq.reader.release(ritem);
        }
    }
}

//------------------------------------------------------------------------------
AQTEST(given_QueueIdClaim_when_QueueIdRetrieve_then_QueueIdMatches)
{
    for (int i = 0; i < 12; ++i)
    {
        vector<unsigned int> quid;
        for (size_t j = 0; j < aq.pageCount() - 1; ++j)
        {
            AQWriterItem witem;
            REQUIRE(aq.writer.claim(witem, aq.pageSize()));
            quid.push_back(witem.queueIdentifier());
            REQUIRE(aq.writer.commit(witem));
        }
        for (size_t j = 0; j < aq.pageCount() - 1; ++j)
        {
            AQItem ritem;
            REQUIRE(aq.reader.retrieve(ritem));
            REQUIRE(ritem.queueIdentifier() == quid[j]);
            aq.reader.release(ritem);
        }
    }
}

//------------------------------------------------------------------------------
AQTEST(given_QueueSeqWraps_when_QueueIdRetrieve_then_QueueIdInsideMask)
{
    set<unsigned int> s;
    for (size_t i = 0; i < aq.pageCount() * (2 + (CtrlOverlay::REF_SEQ_MASK >> CtrlOverlay::REF_SEQ_SHIFT)); ++i)
    {
        AQWriterItem witem;
        REQUIRE(aq.writer.claim(witem, aq.pageSize()));
        REQUIRE((witem.queueIdentifier() & AQItem::QUEUE_IDENTIFIER_MASK) == witem.queueIdentifier());
        uint32_t id = witem.queueIdentifier();
        REQUIRE(aq.writer.commit(witem));

        AQItem ritem;
        REQUIRE(aq.reader.retrieve(ritem));
        REQUIRE(ritem.queueIdentifier() == id);
        aq.reader.release(ritem);
    }
}




//=============================== End of File ==================================
