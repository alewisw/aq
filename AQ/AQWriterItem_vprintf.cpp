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

#include <stdio.h>

using namespace aq;
using namespace std;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// Require at most this many bytes to use alloca() for the buffer allocation.
#define HEURISTIC_MAXIMUM_BYTES_FOR_ALLOCA          256




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
int AQWriterItem::vprintf(size_t off, const char *fmt, va_list argp)
{
    // Validate arguments.
    if (!isAllocated())
    {
        throw domain_error("Cannot write to an AQWriterItem that has not been claimed() or has already been commit()'ed.");
    }
    if (fmt == NULL)
    {
        throw invalid_argument("Cannot printf with fmt set to NULL");
    }

    // Calculate the space we need.
    va_list cargp = argp;
    int count = _vscprintf(fmt, cargp);
    if (count <= 0)
    {
        return 0;
    }

    // Find the item into which we are going to print.
    size_t memSize = 0;
    AQWriterItem *item = writeAdvance(off, memSize, WRITE_PARTIAL);
    if (item == NULL)
    {
        return ~0;
    }

    size_t reqLen = (size_t)count;
    size_t avail = (m_writer->isExtendable() ? item->capacity() : item->size()) - off;
    if (avail >= reqLen)
    {
        // We can print directly into the item.
        _vsnprintf((char *)&(*item)[off], reqLen, fmt, argp);

        // Output was not trunctated - we've fully populated the buffer.
        // Update the item on the basis that 'count' bytes have been
        // added.
        off += reqLen;
        if (m_writer->isExtendable())
        {
            if (item->m_memSize < off)
            {
                item->m_memSize = off;
            }
        }
        else
        {
            if (item->m_accumulator < off)
            {
                item->m_accumulator = off;
            }
        }
    }
    else
    {
        // Not enough space available - format into a buffer then write into 
        // the item instead.
        size_t actLen;
        if (reqLen <= HEURISTIC_MAXIMUM_BYTES_FOR_ALLOCA)
        {
            // Small enough that we can safely use the stack rather than the heap.
            char *buf = (char *)alloca(reqLen);
            _vsnprintf(buf, reqLen, fmt, argp);
            actLen = write(buf, reqLen, AQWriterItem::WRITE_PARTIAL);
        }
        else
        {
            // Too large for the stack - use the heap to store the buffer.
            char *buf = (char *)malloc(reqLen);
            _vsnprintf(buf, reqLen, fmt, argp);
            actLen = write(buf, reqLen, AQWriterItem::WRITE_PARTIAL);
            free(buf);
        }
        if (actLen < reqLen)
        {
            count = ~((int)actLen);
        }
    }
    return count;
}




//=============================== End of File ==================================
