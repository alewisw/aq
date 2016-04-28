#ifndef TRACEORDER_H
#define TRACEORDER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Atomic.h"
#include "TraceBuffer.h"

#include <iostream>
#include <string>
#include <vector>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// The default buffer size for the trace buffers.
#define TRACE_MANAGER_BUFFER_SIZE_DEFAULT   1000




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// The manager for a collection of trace buffers.
namespace aq { class TraceManager
{
public:

    // The possible record content logging options.
    enum LogContentMode
    {
        // Don't log the content.
        None,

        // Log the content as a string.
        String,

        // Each log entry contains a single unsigned 32-bit integer.
        Bytes,
    };

    // Constructs a new trace manager specifying the maximum number of records 
    // to keep in each trace buffer.
    //
    // The queueControlSize is the number of bytes to allocate to capturing queue
    // state; if set to 0 queue state is not captured.
    TraceManager(LogContentMode logContent = None,
                 size_t bufferSize = TRACE_MANAGER_BUFFER_SIZE_DEFAULT);

    // Not defined; trace managers cannot be copied or assigned.
    TraceManager(const TraceManager& other);
    TraceManager& operator=(const TraceManager& other);

    // Destroys this trace manager.
    virtual ~TraceManager(void);

private:

    // The content logging mode.
    const LogContentMode m_logContent;

    // The size of each trace buffer.
    const size_t m_bufferSize;

    // The tracing order number.
    volatile uint64_t m_order;

    // The buffers within this trace manager.
    std::vector<TraceBuffer *> m_buffers;

public:

    // Clears all the trace buffers in this trace manager.
    void clear(void);

    // Gets the content logging mode for this trace manager.
    LogContentMode logContent(void) const { return m_logContent; }

    // Returns the size of the buffers for this trace manager.
    size_t bufferSize(void) const { return m_bufferSize; }

    // Creates a new trace buffer with the passed 'id'.
    TraceBuffer *createBuffer(const std::string &id, size_t recordCapacity = 0);

    // Obtains the next trace order number from this trace manager.
    uint64_t nextOrderNumber(void) { return Atomic::increment(&m_order); }

    // Writes the trace buffers to the passed output stream.
    void write(std::ostream& os) const;

};}




#endif
//=============================== End of File ==================================
