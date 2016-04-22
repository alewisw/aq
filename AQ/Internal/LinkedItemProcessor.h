#ifndef EXTENDLINKPROCESSOR_H
#define EXTENDLINKPROCESSOR_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQItem.h"

#include <map>




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

// An LinkedItemProcessor takes in individual reader items and constructs the 
// linked reader items needed to create multi-item records.
namespace aq { class LinkedItemProcessor
{
public:

    // Constructs a new extend link processor.
    LinkedItemProcessor(void);

    // Not implemented; LinkedItemProcessor does not support copy construction 
    // or assignment opertions.
    LinkedItemProcessor(const LinkedItemProcessor& other);
    LinkedItemProcessor& operator=(const LinkedItemProcessor& other);

    // Destroys this extend link processor.
    ~LinkedItemProcessor(void);

    // Resets all internal state tracking for this processor.
    void reset(void);

    // The four possible outcomes from processing an item.
    enum Outcome
    {
        // The item is entirely invalid, discard it.
        DISCARD,

        // The item is valid and is part of a larger collection.  It
        // has been consumed. 
        CONSUMED,

        // The item has been updated to the next item to process.  This
        // may mean that the entire content of 'item' has been replaced.
        PRODUCED,
    };

    // Takes the passed item 'item' and processes it in the extend link 
    // processor.
    //
    // The return value indicates the processing result.  See the
    // Outcome enumeration for details.
    Outcome nextItem(AQItem& item);

private:

    // Called when the entry 'first' needs to be added to the m_incomplete list
    // and it is waiting for 'nextQuid'.  This searches the out-of-order list 
    // for that entry and updates the incomplete list if a match is found.
    //
    // Returns PRODUCED if this resulted in a complete record (item contains that
    // record) or CONSUMED if this did not result in a complete record.
    Outcome processOutOfOrder(uint32_t nextQuid, AQItem *first, AQItem& item);

    // Appends 'newItem' to the end of the list starting at 'first'.
    void appendItem(AQItem *first, AQItem *newItem);

    // Moves the values in 'src' into 'dst', clears 'src' and sets it to m_nextItem.
    void moveItemList(AQItem &src, AQItem& dst);

    // Updates 'item' to contain 'first', deletes 'first' and returns PRODUCED.
    Outcome produceCompleteItem(AQItem *first, AQItem *last, AQItem& item);

    // The map of all incomplete items.  The index is the queue identifier that
    // is required for the next buffer in the item.
    std::map<uint32_t, AQItem *> m_incomplete;

    // The map of items that did not have the 'first' flag set and were retrieved
    // out of order.
    std::map<uint32_t, AQItem *> m_outOfOrder;

};}




#endif
//=============================== End of File ==================================
