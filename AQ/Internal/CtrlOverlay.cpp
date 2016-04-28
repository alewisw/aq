//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "CtrlOverlay.h"

#include "AQItem.h"
#include "AQ.h"

#include <stddef.h>

namespace aq {




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
bool CtrlOverlay::isFormatted(size_t memSize) const
{
    uint32_t magic = ((pageSizeShift << CtrlOverlay::HEADER_XREF_PAGE_SIZE_SHIFT)
            | (memOffset  << CtrlOverlay::HEADER_XREF_MEM_OFFSET_SHIFT)
            | (pageCount));
    return memSize >= size 
        && magic == headerXref 
        && formatVersion == FORMAT_VERSION_1 
        && !(options & OPTION_INVALID_MASK);
}

//------------------------------------------------------------------------------
unsigned char *CtrlOverlay::pageToMem(uint32_t pageNum) const
{
    if (pageNum >= pageCount)
    {
        return NULL;
    }
    else
    {
        unsigned char *ptr = (unsigned char *)this;
        return &ptr[memOffset + (pageNum << pageSizeShift)];
    }
}

//------------------------------------------------------------------------------
uint32_t CtrlOverlay::memToPage(const void *mem) const
{
    unsigned char *ptr = (unsigned char *)this;

    if (mem < ptr)
    {
        // Less than the start address; must be invalid.
        return PAGENUM_INVALID;
    }

    size_t offset = (size_t)((const unsigned char *)mem - ptr) - memOffset;
    if (offset & ((1 << pageSizeShift) - 1))
    {
        // Does not fall directly on a memory page boundary.
        return PAGENUM_INVALID;
    }
    uint32_t pageNum = offset >> pageSizeShift;
    if (pageNum >= pageCount)
    {
        // Cannot be valid as it is after the total number of pages.
        return PAGENUM_INVALID;
    }
    return pageNum;
}

//------------------------------------------------------------------------------
uint32_t CtrlOverlay::sizeToPageCount(size_t sz) const
{
    // Calculate the number of pages required.  We take the requested 
    // size, add the number of bytes in a page subtract 1, then divide
    // by the size of each page - this ensures we round up and get
    // enough space.
    return (sz + (1 << pageSizeShift) - 1) >> pageSizeShift;
}

//------------------------------------------------------------------------------
uint32_t CtrlOverlay::queueRefToIndex(uint32_t ref) const
{
    return ref & REF_INDEX_MASK;
}

//------------------------------------------------------------------------------
uint32_t CtrlOverlay::queueRefIncrement(uint32_t ref, uint32_t amount) const
{
    uint32_t index = ref & REF_INDEX_MASK;
    uint32_t seq = ref & REF_SEQ_MASK;

    index += amount;
    while (index >= pageCount)
    {
        index -= pageCount;
        seq += REF_SEQ_INCR;
    }

    return seq | index;
}

//------------------------------------------------------------------------------
uint32_t CtrlOverlay::availableSequentialPages(uint32_t headIdx, uint32_t tailIdx) const
{
    // The actual number of availableSize pages is given my the maximum 'sequential'
    // number of pages as all allocates are performed in fixed blocks.
    if (tailIdx > headIdx)
    {
        // Only a single availableSize block right in the middle of the queue.
        return tailIdx - headIdx - 1;
    }
    else
    {
        // Two possible blocks - one at the start and one at the end of the
        // queue.  Pick the largest noting that we must always have one
        // unused page.
        uint32_t end = pageCount - headIdx - (tailIdx == 0 ? 1 : 0);
        if (end + 1 > tailIdx)
        {
            return end;
        }
        else
        {
            return tailIdx - 1;
        }
    }
}




}
//=============================== End of File ==================================
