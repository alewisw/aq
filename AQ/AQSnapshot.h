#ifndef AQSNAPSHOT_H
#define AQSNAPSHOT_H
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

#include <cstdint>

#include <vector>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQ;
namespace aq
{
    class TraceBuffer;
    struct CtrlOverlay;
}




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Captures and holds a 'snapshot in time' of a Multi-Producer Allocating 
// Concurrent queue.
class AQSnapshot
{
public:

    // Constructs an empty snapshot object with zero items.
    //
    // The 'trace' argument is used for tracing and logging queue access.
    AQSnapshot(aq::TraceBuffer *trace = NULL);

    // Constructs a new snapshot containing all the valid items in the queue.
    // For a description of what constitutes a valid item see the snap() 
    // function.
    //
    // The 'trace' argument is used for tracing and logging queue access.
    AQSnapshot(const AQ &queue, aq::TraceBuffer *trace = NULL);

    // Constructs a new snapshot that is an exact copy of an existing snapshot.
    AQSnapshot(const AQSnapshot& other);

    // Assigns the value of this snapshot to exactly match another.
    AQSnapshot& operator=(const AQSnapshot& other);

    // Destroys this snapshot.
    virtual ~AQSnapshot(void);

    // Takes a snapshot of the passed queue, storing it in this object.
    void snap(const AQ& queue);

    // The four stages of snapshot capture; these are called from snap() in
    // the order declared below.
    //
    // These have been made available for the purpose of unit testing and should
    // never be directly called by an application.  Always use 'snap()'.
    void snap1InitialHead(const AQ& queue);
    void snap2InitialCtrlq(void);
    void snap3PageMemory(void);
    void snap4FinalHead(void);

private:

    // Resets this snapshot so that it contains no items.
    void reset(void);

    // Clones the data from the object 'other' into this object.  The memory 
    // for this object must already have been free'd.
    void clone(const AQSnapshot& other);

    // The trace buffer for this snapshot.
    aq::TraceBuffer *m_trace;

    // The source control overlay that is being captured.
    aq::CtrlOverlay *m_srcCtrl;

    // The initial head value captured
    uint32_t m_initHeadRef;

    // The final head value captured
    uint32_t m_finalHeadRef;

    // The memory for this snapshot including the initial control queue.
    unsigned char *m_mem;

    // The memory size that has been allocated.
    size_t m_memSize;

    // The set of items decoded from the queue and made availableSize in this 
    // snapshot.
    AQItem *m_items;

    // The number of items actually found in m_items.
    size_t m_itemCount;

public:

    // The initial and final head references from the snapshot capture.
    uint32_t initHeadRef(void) const { return m_initHeadRef; }
    uint32_t finalHeadRef(void) const { return m_finalHeadRef; }

    // The number of items in this snapshot.
    size_t size(void) const { return m_itemCount; }

    // Gets an item from this snapshot.
    const AQItem& operator[](size_t idx) const { return m_items[idx]; }

};




#endif
//=============================== End of File ==================================
