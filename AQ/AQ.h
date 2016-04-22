#ifndef AQ_H
#define AQ_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <cstdint>
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

// Forwrd declarations.
namespace aq 
{
    class TestPointNotifier;
    class TraceBuffer;
    struct CtrlOverlay;
}
class AQItem;




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Base class for a Multi-Producer Allocating Concurrent queue.  This gives 
// basic information about the queue and encapsulates its memory, however it 
// does not provide any mechanism to read or write the queue.  This is done by
// constructing any number of MpcaQueueWriter objects for writing, and one
// AQReader object for reading the queue.
class AQ
{
    // The snapshot class may access the m_ctrl and m_memSize fields.
    friend class AQSnapshot;

    // The trace buffer captures data directly from within the MPAC queue for
    // logging purposes.
    friend class aq::TraceBuffer;

protected:

    // Constructs a queue object that uses the passed shared memory region 'mem'
    // of total size 'memSize' bytes.  This does not read or write the memory -
    // it just sets up the internal pointers.
    // 
    // The 'testPointCount' argument specifies the number of test points used by
    // the automated unit tests availableSize for this particular interface to the
    // queue.
    //
    // The 'trace' argument is used for tracing and logging queue access.
    AQ(int testPointCount, void *mem, size_t memSize, 
              aq::TraceBuffer *trace = NULL);

    // Constructs this queue as an exact copy of another queue.
    AQ(const AQ& other);

    // Assigns the value of this queue to exactly match another.
    AQ& operator=(const AQ& other);

    // Destroys this queue.
    virtual ~AQ(void);

protected:

    // The trace buffer for this queue.
    aq::TraceBuffer *m_trace;

public:

    // The CRC-32 option flag.
    static const unsigned int OPTION_CRC32 = 1 << 0;

    // The link identifier control word option flag.
    static const unsigned int OPTION_LINK_IDENTIFIER = 1 << 1;

    // The extendable items control word option flag.  Implies 
    // (and sets) OPTION_LINK_IDENTIFIER.
    static const unsigned int OPTION_EXTENDABLE = 1 << 2;

public:

    // Returns true if the memory for this queue has been formatted and the queue
    // can be accessed.
    bool isFormatted(void) const;

    // Returns true if this queue has been formatted in extendable mode.
    bool isExtendable(void) const;

    // Returns the size of the memory that was passed to this queue.
    size_t memorySize(void) const { return m_memSize;  }

    // Returns the size of each page in the memory queue; if the memory is not
    // formatted then 0 is returned.  When allocating memory from the queue 
    // using an integer multiple of pageSize() provides the most efficient 
    // allocation policy.
    size_t pageSize(void) const;

    // Returns the number of pages in the queue.
    size_t pageCount(void) const;

    // Returns the total number of bytes currently availableSize for a producer
    // to write into this queue in a single item.
    size_t availableSize(void) const;

    // Returns the number of contention events detected during claim().
    uint32_t claimContentionCount(void) const;

protected:

    // Throws an AQUnformattedException if this queue is not headerXref; if
    // the queue is headerXref returns the m_ctrl object.
    aq::CtrlOverlay *ctrlThrowOnUnformatted(const char *func) const;

    // Defines the control region of the shared memory.
    aq::CtrlOverlay *m_ctrl;

    // The total size, as passed into this class, of the shared memory.
    size_t m_memSize;


    // Test point management - only used as part of the automated unit testing
    // framework.
#ifdef AQ_TEST_POINT
protected:
    aq::TestPointNotifier *m_tpn;
    void testPoint(int tp);
public:
    aq::TestPointNotifier *testPointNotifier(void) const { return m_tpn;  }
#else
protected:
    void testPoint(int tp) { }
#endif

};




#endif
//=============================== End of File ==================================
