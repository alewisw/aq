#ifndef TRACEBUFFER_H
#define TRACEBUFFER_H
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

#include <string>
#include <stdarg.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// The number of bytes allocated to each trace buffer message.
#define TRACE_BUFFER_MSG_SIZE           250

#ifdef AQ_TEST_TRACE

// Baseline tracing macro used to build all other macro types.
#define TRACE_ITEM_TYPE(__type, __ctrl, __item, __logAllItems, __logItemData,  \
                        __data, __dataSize, ...)                                \
do                                                                              \
{                                                                               \
    if (m_trace != NULL)                                                        \
    {                                                                           \
        m_trace->write(__FUNCTION__, __LINE__, TraceBuffer::Record::__type,     \
                       (__ctrl), (__item), (__logAllItems), (__logItemData),    \
                       (__data), (__dataSize), "" __VA_ARGS__);                 \
    }                                                                           \
} while(0)

#else
// Tracing stubs for when tracing has been disabled.
#define TRACE_ITEM_TYPE(__item, __type, ...)

#endif

#define TRACE_DATA_ENTRY(__data, __dataSize, ...)       TRACE_ITEM_TYPE(Entry,   NULL,   NULL, false, false, __data, __dataSize, __VA_ARGS__)
#define TRACE_ITEMDATA_ENTRY(__ctrl, __item, ...)       TRACE_ITEM_TYPE(Entry, __ctrl, __item,  true, true,   NULL,          0, __VA_ARGS__)
#define TRACE_ITEM_ENTRY(__ctrl, __item, ...)           TRACE_ITEM_TYPE(Entry, __ctrl, __item,  true, false,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEMDATA_ENTRY(__ctrl, __item, ...)      TRACE_ITEM_TYPE(Entry, __ctrl, __item, false, true,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEM_ENTRY(__ctrl, __item, ...)          TRACE_ITEM_TYPE(Entry, __ctrl, __item, false, false,   NULL,          0, __VA_ARGS__)
#define TRACE_CTRL_ENTRY(__ctrl, ...)                   TRACE_ITEM_ENTRY(__ctrl, NULL, __VA_ARGS__)
#define TRACE_ENTRY(...)                                TRACE_CTRL_ENTRY(NULL, __VA_ARGS__)

#define TRACE_DATA_EXIT(__data, __dataSize, ...)        TRACE_ITEM_TYPE(Exit,   NULL,   NULL, false, false, __data, __dataSize, __VA_ARGS__)
#define TRACE_ITEMDATA_EXIT(__ctrl, __item, ...)        TRACE_ITEM_TYPE(Exit, __ctrl, __item,  true,  true,   NULL,          0, __VA_ARGS__)
#define TRACE_ITEM_EXIT(__ctrl, __item, ...)            TRACE_ITEM_TYPE(Exit, __ctrl, __item,  true, false,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEMDATA_EXIT(__ctrl, __item, ...)       TRACE_ITEM_TYPE(Exit, __ctrl, __item, false,  true,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEM_EXIT(__ctrl, __item, ...)           TRACE_ITEM_TYPE(Exit, __ctrl, __item, false, false,   NULL,          0, __VA_ARGS__)
#define TRACE_CTRL_EXIT(__ctrl, ...)                    TRACE_ITEM_EXIT(__ctrl, NULL, __VA_ARGS__)
#define TRACE_EXIT(...)                                 TRACE_CTRL_EXIT(NULL, __VA_ARGS__)

#define TRACE_DATA_ENTRYEXIT(__data, __dataSize, ...)   TRACE_ITEM_TYPE(EntryExit,   NULL,   NULL, false, false, __data, __dataSize, __VA_ARGS__)
#define TRACE_ITEMDATA_ENTRYEXIT(__ctrl, __item, ...)   TRACE_ITEM_TYPE(EntryExit, __ctrl, __item,  true,  true,   NULL,          0, __VA_ARGS__)
#define TRACE_ITEM_ENTRYEXIT(__ctrl, __item, ...)       TRACE_ITEM_TYPE(EntryExit, __ctrl, __item,  true, false,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEMDATA_ENTRYEXIT(__ctrl, __item, ...)  TRACE_ITEM_TYPE(EntryExit, __ctrl, __item, false,  true,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEM_ENTRYEXIT(__ctrl, __item, ...)      TRACE_ITEM_TYPE(EntryExit, __ctrl, __item, false, false,   NULL,          0, __VA_ARGS__)
#define TRACE_CTRL_ENTRYEXIT(__ctrl, ...)               TRACE_ITEM_ENTRYEXIT(__ctrl, NULL, __VA_ARGS__)
#define TRACE_ENTRYEXIT(...)                            TRACE_CTRL_ENTRYEXIT(NULL, __VA_ARGS__)

#define TRACE_DATA_INVALID(__data, __dataSize, ...)     TRACE_ITEM_TYPE(Invalid,   NULL,   NULL, false, false, __data, __dataSize, __VA_ARGS__)
#define TRACE_ITEMDATA_INVALID(__ctrl, __item, ...)     TRACE_ITEM_TYPE(Invalid, __ctrl, __item,  true,  true,   NULL,          0, __VA_ARGS__)
#define TRACE_ITEM_INVALID(__ctrl, __item, ...)         TRACE_ITEM_TYPE(Invalid, __ctrl, __item,  true, false,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEMDATA_INVALID(__ctrl, __item, ...)    TRACE_ITEM_TYPE(Invalid, __ctrl, __item, false,  true,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEM_INVALID(__ctrl, __item, ...)        TRACE_ITEM_TYPE(Invalid, __ctrl, __item, false, false,   NULL,          0, __VA_ARGS__)
#define TRACE_CTRL_INVALID(__ctrl, ...)                 TRACE_ITEM_INVALID(__ctrl, NULL, __VA_ARGS__)
#define TRACE_INVALID(...)                              TRACE_CTRL_INVALID(NULL, __VA_ARGS__)

#define TRACE_DATA(__data, __dataSize, ...)             TRACE_ITEM_TYPE(Log,   NULL,   NULL, false, false, __data, __dataSize, __VA_ARGS__)
#define TRACE_ITEMDATA(__ctrl, __item, ...)             TRACE_ITEM_TYPE(Log, __ctrl, __item,  true,  true,   NULL,          0, __VA_ARGS__)
#define TRACE_ITEM(__ctrl, __item, ...)                 TRACE_ITEM_TYPE(Log, __ctrl, __item,  true, false,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEMDATA(__ctrl, __item, ...)            TRACE_ITEM_TYPE(Log, __ctrl, __item, false,  true,   NULL,          0, __VA_ARGS__)
#define TRACE_1ITEM(__ctrl, __item, ...)                TRACE_ITEM_TYPE(Log, __ctrl, __item, false, false,   NULL,          0, __VA_ARGS__)
#define TRACE_CTRL(__ctrl, ...)                         TRACE_ITEM(__ctrl, NULL, __VA_ARGS__)
#define TRACE(...)                                      TRACE_CTRL(NULL, __VA_ARGS__)

// Breaks up a reference 'r' into its constituent parts for display using 
// TRACE_REF_FMT.
#define TRACE_REF(r)                                                            \
    (((r) & CtrlOverlay::REF_SEQ_MASK) >> CtrlOverlay::REF_SEQ_SHIFT),          \
    ((r) & CtrlOverlay::REF_INDEX_MASK)
#define TRACE_REF_FMT                   "%u:%u"

// Breaks up a reference 'r' to a set of pages of length 'l' into its constituent 
// parts for display using TRACE_PGS_FMT.
#define TRACE_PGS(r, l)                                                         \
    (((r) & CtrlOverlay::REF_SEQ_MASK) >> CtrlOverlay::REF_SEQ_SHIFT),          \
    ((r) & CtrlOverlay::REF_INDEX_MASK),                                        \
    (((r) & CtrlOverlay::REF_INDEX_MASK) + l - 1)
#define TRACE_PGS_FMT                   "%u:%u-%u"

// Breaks up a control queue vale 'c' into its constituent parts for display using
// TRACE_CTQ_FMT.
#define TRACE_CTQ(c)                                                            \
    (((c) & CtrlOverlay::CTRLQ_CLAIM_MASK) ? 'c' : '-'),                        \
    (((c) & CtrlOverlay::CTRLQ_COMMIT_MASK) ? 'C' : '-'),                       \
    (((c) & CtrlOverlay::CTRLQ_DISCARD_MASK) ? 'D' : '-'),                      \
    (((c) & CtrlOverlay::CTRLQ_SEQ_MASK) >> CtrlOverlay::REF_SEQ_SHIFT),        \
     ((c) & CtrlOverlay::CTRLQ_SIZE_MASK)
#define TRACE_CTQ_FMT                   "%c%c%c:%u:%u"




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQItem;

namespace aq 
{
    struct CtrlOverlay;
    class TraceManager;
}




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Contains a buffer used to capture trace information for a particular queue
// access.
namespace aq { class TraceBuffer
{
public:

    // Constructs a new trace buffer with the identifier 'id'.  The 'order' is a
    // global tracing order counter used to create a merged view of muliple
    // trace buffers if an error is detected.
    //
    // The record capacity is 'recordCapacity', unless 0 is used, in which case
    // the default capacity from the manager is used. 
    TraceBuffer(TraceManager& mgr, const std::string &id, size_t recordCapacity = 0);

    // Not defined; trace buffers cannot be copied or assigned.
    TraceBuffer(const TraceBuffer& other);
    TraceBuffer& operator=(const TraceBuffer& other);

    // Destroys this trace buffer.
    virtual ~TraceBuffer(void);

private:

    // The manger fort his trace buffer.
    TraceManager& m_mgr;

    // The identifier for this buffer.
    const std::string m_id;

public:

    // Clears all the records in this trace buffer.
    void clear(void);

    // Defines a record in this trace buffer.
    struct Record
    {
        // Defines the context for a trace request.
        enum Context
        {
            // Traces an invalid argument call to a function.
            Invalid,

            // Tracing the entry to a function.
            Entry,

            // Tracing both an entry and an exit.
            EntryExit,

            // Tracing inside a function.
            Log,

            // Tracing the exit from a function.
            Exit,
        };

        // The order for this record.
        uint64_t order;

        // The function from which this record was created.
        const char *function;

        // The line in the function from which this record was created.
        int line;

        // The context for this record.
        Context context;

        // Set to true if a CtrlOverlay was provided.
        bool hasCtrl;

        // The head and tail reference captured when a CtrlOverlay is
        // provided; 
        uint32_t headRef;
        uint32_t tailRef;

        // The message itself.
        char msg[TRACE_BUFFER_MSG_SIZE];

        // The binary data associated with the message. This is a pointer 
        // into the 'msg' buffer.
        unsigned char *data;

        // The number of data bytes in the data array.
        size_t dataSize;

        // Set to true if the data has been truncated.
        bool dataTruncated;

    };

    // Writes a record into this record buffer for the function 'function' at line
    // 'line'.  The record has a context of 'context' and a message given by 'fmt' 
    // with arguments '...'.
    //
    // The other arguments are used to contextualize the log record:
    //  'ctrl' is the active control overlay for the queue.
    //  'item' is the item currently being processed.
    //  'logAllItems' if true then the entire chain of items is logged,
    //                if false only the single item passed.
    //  'logItemData' if true then the item data is logged if false then
    //                no item data is logged.
    //  'data' if non-NULL a pointer to the data to log.
    //  'dataSize' is the maximum number of bytes from 'data' to log.
    //
    // The record originated with the queue given by 'queue'.
    void write(const char *function, int line, Record::Context context, 
               aq::CtrlOverlay *ctrl, const AQItem *item, bool logAllItems,
               bool logItemData, const void *data, size_t dataSize,
               const char *fmt, ...);

private:

    // Prints the passed item into the log record.
    void printItem(size_t& pos, aq::CtrlOverlay *ctrl, Record *rec, int idx,
                   const AQItem *item);

    // Provides a mechanism to print into the 'msg' buffer of a record that is safe from
    // buffer overflow.  
    //
    // 'pos' tracks the current append position in the buffer.  It must start at 0.
    // On return it points to the nul terminator.
    void sprintfRecord(size_t& pos, Record *rec, const char *fmt, ...);
    void vsprintfRecord(size_t& pos, Record *rec, const char *fmt, va_list argp);

    // The array of records.
    Record *m_records;

    // The capacity of the record array.
    const size_t m_recordCapacity;

    // The index of the first record in the array.
    size_t m_recordFirst;

    // The number of records in the array.
    size_t m_recordCount;

public:

    // Writes the passed record 'rec' to the output stream 'os'.
    void writeRecord(std::ostream& os, const TraceBuffer::Record& rec) const;

    // Returns the identifier for this trace buffer.
    const std::string &id(void) const { return m_id; }

    // Returns the number of records in this buffer.
    size_t recordCount(void) const { return m_recordCount; }

    // Returns one of the records from this record array at index 'idx'.
    const Record &record(size_t idx) const { return m_records[(m_recordFirst + idx) % m_recordCapacity]; }

};}




#endif
//=============================== End of File ==================================
