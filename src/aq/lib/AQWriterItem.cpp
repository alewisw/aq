//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQWriterItem.h"

#include "AQWriter.h"

#include <string.h>
#include <sstream>
#include <stdexcept>

using namespace aq;
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
size_t AQWriterItem::write(size_t off, const void *mem, size_t memSize, 
    InsufficientSpaceAction insufficientSpaceAction)
{
    if (!isAllocated())
    {
        throw domain_error("Cannot write to an AQWriterItem that has not been claimed() or has already been commit()'ed.");
    }
    if (mem == NULL && memSize != 0)
    {
        throw invalid_argument("Cannot write with mem set to NULL");
    }

    // Find the item that is to be updated and set the offset to an offset into
    // that item.
    AQWriterItem *item = writeAdvance(off, memSize, insufficientSpaceAction);
    if (item == NULL)
    {
        return 0;
    }

    // Perform the series of memcpy() operations to write into the item.
    const unsigned char *buf = (const unsigned char *)mem;
    size_t retSize = memSize;
    while (memSize > 0)
    {
        size_t avail = item->capacity() - off;
        if (avail > memSize)
        {
            avail = memSize;
        }
        memcpy(&item->m_mem[off], buf, avail);
        off = 0;
        buf += avail;
        memSize -= avail;
        item = item->next();
    }
    return retSize;
}

//------------------------------------------------------------------------------
size_t AQWriterItem::currentOffset(void) const
{
    if (!isAllocated())
    {
        throw domain_error("Cannot write to an AQWriterItem that has not been claimed() or has already been commit()'ed.");
    }

    size_t off = last()->m_accumulator;
    if (m_writer->isExtendable())
    {
        off += last()->m_memSize;
    }
    return off;
}

//------------------------------------------------------------------------------
AQWriterItem *AQWriterItem::writeAdvance(size_t& off, size_t& memSize,
    InsufficientSpaceAction insufficientSpaceAction)
{
    if (!m_writer->isExtendable())
    {
        return writeAdvanceNormal(off, memSize, insufficientSpaceAction);
    }
    else
    {
        return writeAdvanceExtendable(off, memSize, insufficientSpaceAction);
    }
}

//------------------------------------------------------------------------------
AQWriterItem *AQWriterItem::writeAdvanceNormal(size_t off, size_t& memSize,
    InsufficientSpaceAction insufficientSpaceAction)
{
    // Offset exceeds the capacity of the item.
    if (off >= capacity())
    {
        memSize = 0;
        if (insufficientSpaceAction == WRITE_PARTIAL)
        {
            m_accumulator = capacity();
            return this;
        }
        else
        {
            return NULL;
        }
    }

    // Offset is less than the capacity of the item.
    size_t acc = off + memSize;
    if (acc > capacity())
    {
        // Offset + size exceed the capacity of the item.
        acc = capacity();
        if (insufficientSpaceAction == WRITE_PARTIAL)
        {
            m_accumulator = capacity();
            memSize = capacity() - off;
            return this;
        }
        else
        {
            return NULL;
        }
    }

    // Offset + size do not exceed the capacity of the item, all is well.
    if (acc > m_accumulator)
    {
        m_accumulator = acc;
    }
    return this;
}

//------------------------------------------------------------------------------
AQWriterItem *AQWriterItem::writeAdvanceExtendable(size_t& off, size_t& memSize,
    InsufficientSpaceAction insufficientSpaceAction)
{
    AQWriterItem *last = this->last();
    AQWriterItem *item;
    size_t rtotal = 0;

    // Extendable item - find the item at offset 'off' from this item and
    // adjust off to an offset within that item.
    if (off < capacity())
    {
        item = this;
    }
    else
    {
        // Go to the last item as we usually append.  Find the item that matches.
        //
        // Example:
        //   Item          #1 #2 #3 #4 #5
        //   Capacity:    | 4| 8| 4|16| 4|
        //   Size:        | 4| 8| 4|16| 2|
        //   Accumulator: | 0| 4|12|16|32 
        //                   
        //   (a) Item(S) #1, offset 30 --> Item(W) #4, offset 14 --> off + S(A) - W(A) = 30 +  0 - 16 = 14  
        //   (b) Item(S) #2, offset 26 --> Item(W) #4, offset 14 --> off + S(A) - W(A) = 26 +  4 - 16 = 14
        //   (c) Item(S) #3, offset 18 --> Item(W) #4, offset 14 --> off + S(A) - W(A) = 18 + 12 - 16 = 14
        off += m_accumulator;
        for (item = last; off < item->m_accumulator; item = item->prev())
        {
            rtotal += item->capacity();
        }
        off -= item->m_accumulator;
    }

    // Determine if we have enough space or if we need to extend.  If we
    // need to extend then do it or fail in the attempt.
    //
    // Example:
    //   Item          #1 #2 #3 #4 #5
    //   Capacity:    | 4| 8| 4|16| 4|
    //   Size:        | 4| 8| 4|16| 2
    //   Accumulator: | 0| 4|12|16|32|
    //   rtotal:      |32|24|20| 4| 0|
    //                                          rtotal + I(C) - off = avail - memSize       L(C) lastSize
    //   (a) Item(I) #5, offset 1, size  3 -->       0 +    4 -   1 =  3    -       3 = 0   < L(C) => L(S) = L(C) - 0 = 4
    //   (c) Item(I) #4, offset 6, size 14 -->       4 +   16 -   6 = 14    -      14 = 0   < L(C) => L(S) = L(C) - 0 = 4
    //   (c) Item(I) #4, offset 6, size  9 -->       4 +   16 -   6 = 14    -       9 = 5  >= L(C) => L(S) = 0
    //   (c) Item(I) #4, offset 6, size 10 -->       4 +   16 -   6 = 14    -      10 = 4  >= L(C) => L(S) = 0
    //   (c) Item(I) #4, offset 6, size 11 -->       4 +   16 -   6 = 14    -      11 = 3   < L(C) => L(S) = L(C) - 3 = 1
    //   (e) Item(I) #1, offset 1, size 35 -->      32 +    4 -   1 = 35    -      35 = 0   < L(C) => L(S) = L(C) - 0 = 4
    //   (b) Item(I) #5, offset 1, size  4 -->       0 +    4 -   1 =  3    4 >  3 ==> EXTEND
    //   (d) Item(I) #4, offset 7, size 14 -->       4 +   16 -   7 = 13   14 > 13 ==> EXTEND
    //   (f) Item(I) #1, offset 2, size 35 -->      32 +    4 -   2 = 34   35 > 34 ==> EXTEND
    rtotal += item->capacity();
    if (rtotal < off || memSize > rtotal - off)
    {
        // Extend the list.  This updates all sizes appropriatly.
        size_t extendSize = memSize + off - rtotal;
        if (!extend(extendSize, insufficientSpaceAction))
        {
            if (insufficientSpaceAction != WRITE_PARTIAL)
            {
                // Partial write is not permitted, just fail.
                return NULL;
            }

            // Extend failed but partial write permitted; update the memory size 
            // to reflect the available space.
            last->m_memSize = last->capacity();
            if (off >= last->capacity())
            {
                memSize = 0;
            }
            else
            {
                memSize = last->capacity() - off;
            }
            item = last;
        }
        else
        {
            // Did we allocate enough space in the last item?
            size_t avail = this->last()->capacity();
            if (avail < extendSize)
            {
                size_t diff = extendSize - avail;
                if (diff < memSize)
                {
                    memSize -= diff;
                }
                else
                {
                    memSize = 0;
                }
                if (off >= memSize)
                {
                    return NULL;
                }
            }

            // The offset could be beyond the original last item - if that was the case we
            // need to adjust item to the last item, and offset appropriatly.
            if (rtotal < off)
            {
                off -= last->capacity();
                item = this->last();
            }
        }
    }
    else
    {
        // Update the size of the last item in the list if we reach into its content.
        size_t n = rtotal - off - memSize;
        size_t c = last->capacity();
        size_t lastSize = n < c ? (c - n) : 0;
        if (last->m_memSize < lastSize)
        {
            last->m_memSize = lastSize;
        }
    }
    return item;
}

//------------------------------------------------------------------------------
bool AQWriterItem::extend(size_t memSize, InsufficientSpaceAction insufficientSpaceAction)
{
    AQWriter::ClaimSizeRule claimRule = insufficientSpaceAction == AQWriterItem::WRITE_PARTIAL ? AQWriter::CLAIM_AT_MOST : AQWriter::CLAIM_EXACT;
    AQWriterItem *newItem = new AQWriterItem;
    if (!m_writer->claim(*newItem, memSize, claimRule))
    {
        // Not enough space available.
        delete newItem;
        return false;
    }

    AQWriterItem *first = this->first();
    AQWriterItem *last = this->last();

    // Attach the new item to the end of the list.
    newItem->m_first = first;
    newItem->m_prev = last;
    last->m_next = newItem;
    first->m_prev = newItem;

    // Adjust the sizes of the now next-to-last and last item.
    last->m_memSize = last->capacity();
    newItem->m_accumulator = last->m_accumulator + last->m_memSize;

    newItem->m_memSize = std::min(memSize, newItem->capacity());

    return true;
}




//=============================== End of File ==================================
