#ifndef AQSTRAWMAN_H
#define AQSTRAWMAN_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "CriticalSection.h"
#include "Mutex.h"

#include "IAQReader.h"
#include "IAQWriter.h"

#include <deque>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQItem;
class AQWriterItem;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates an event that can be used to block a thread until it becomes
// set.
class AQStrawManBase
{
public:

    // Constructs an allocating deque.
    AQStrawManBase(int pageSizeShift, size_t pageCount);

private:

    // No implementation is defined for these functions.
    AQStrawManBase(const AQStrawManBase& other);
    AQStrawManBase& operator=(const AQStrawManBase& other);

public:

    // Destroys this event.
    virtual ~AQStrawManBase(void);

    // Resets the queue to all empty.
    void reset(void);

    // The claim() function as defined in IAQWriter.
    bool claim(AQWriterItem& item, size_t memSize);

    // The commit() function as defined in IAQWriter.
    bool commit(AQWriterItem& item);

    // The retrieve() function as defined in IAQReader.
    bool retrieve(AQItem& item);

    // The release() function as defined in IAQReader.
    void release(AQItem& item);

protected:

    // Protects the free lock and the committed lock.
    virtual void freeLock(void) = 0;
    virtual void freeUnlock(void) = 0;
    virtual void committedLock(void) = 0;
    virtual void committedUnlock(void) = 0;

private:

    // The page count.
    const size_t m_pageCount;

    // The size of each page is 1 << 'm_pageSizeShift'.
    const int m_pageSizeShift;

    // The memory to deallocate at the end of the operation.
    unsigned char *m_mem;

    // The page.
    struct Page
    {
        // The size of this page.
        unsigned int m_memSize;

        // The memory for this page.
        unsigned char m_mem[1];
    };

    // The queue of free recods.
    std::deque<Page *> m_free;

    // The queue of committed recods.
    std::deque<Page *> m_committed;

    // Converts a memory pointer into a page pointer.
    Page *memToPage(unsigned char *mem) const;

public:

    // Gets the page count for this queue.
    size_t pageCount(void) const { return m_pageCount; }

    // The size of each page in the queue.
    size_t pageSize(void) const { return (size_t)(1 << m_pageSizeShift); }

};

// Encapsulates an event that can be used to block a thread until it becomes
// set.
template <typename TLock> class AQStrawMan : public AQStrawManBase
{
public:

    // Constructs an allocating deque.
    AQStrawMan(int pageSizeShift, size_t pageCount) : AQStrawManBase(pageSizeShift, pageCount) { };

private:

    // No implementation is defined for these functions.
    AQStrawMan(const AQStrawMan<TLock>& other);
    AQStrawMan<TLock>& operator=(const AQStrawMan<TLock>& other);

public:

    // Destroys this event.
    virtual ~AQStrawMan(void) { };

protected:

    // Protects the free lock and the committed lock.
    virtual void freeLock(void) { m_freeLock.lock(); }
    virtual void freeUnlock(void) { m_freeLock.unlock(); }
    virtual void committedLock(void) { m_committedLock.lock(); }
    virtual void committedUnlock(void) { m_committedLock.unlock(); }

private:

    // The mutex to protect the deque.
    TLock m_freeLock;
    TLock m_committedLock;
    
};




#endif
//=============================== End of File ==================================
