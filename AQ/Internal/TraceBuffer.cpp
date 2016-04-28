//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "TraceBuffer.h"

#include "AQ.h"
#include "AQItem.h"

#include "CtrlOverlay.h"
#include "TraceManager.h"

#include <cstdarg>
#include <cstring>
#include <iomanip>

using namespace std;
using namespace aq;



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

namespace aq {

//------------------------------------------------------------------------------
TraceBuffer::TraceBuffer(TraceManager& mgr, const string &id, size_t recordCapacity)
    : m_mgr(mgr)
    , m_id(id)
    , m_recordCapacity(recordCapacity == 0 ? mgr.bufferSize() : recordCapacity)
    , m_recordFirst(0)
    , m_recordCount(0)
{
    m_records = new Record[m_recordCapacity];
}

//------------------------------------------------------------------------------
TraceBuffer::~TraceBuffer(void)
{
    delete[] m_records;
}

//------------------------------------------------------------------------------
void TraceBuffer::clear(void)
{
    m_recordFirst = 0;
    m_recordCount = 0;
}
//------------------------------------------------------------------------------
void TraceBuffer::write(const char *function, int line, Record::Context context,
                        aq::CtrlOverlay *ctrl, const AQItem *item, bool logAllItems, 
                        bool logItemData, const void *data, size_t dataSize, 
                        const char *fmt, ...)
{
    Record *rec = NULL;
    size_t idx;

    if (m_recordCount < m_recordCapacity)
    {
        idx = m_recordCount++;
    }
    else
    {
        idx = m_recordFirst++;
        if (m_recordFirst == m_recordCapacity)
        {
            m_recordFirst = 0;
        }
    }
    rec = &m_records[idx];

    rec->order = m_mgr.nextOrderNumber();
    rec->function = function;
    rec->line = line;
    rec->context = context;

    if (ctrl != NULL)
    {
        rec->hasCtrl = true;
        rec->headRef = Atomic::read(&ctrl->headRef);
        rec->tailRef = Atomic::read(&ctrl->tailRef);
    }
    else
    {
        rec->hasCtrl = false;
    }
    
    size_t pos = 0;
    if (item != NULL)
    {
        if (!item->isAllocated())
        {
            sprintfRecord(pos, rec, "(unallocated item) ");
        }
        else if (!logAllItems)
        {
            printItem(pos, ctrl, rec, -1, item);
            sprintfRecord(pos, rec, " ");
        }
        else
        {
            int idx = 0;
            for (const AQItem *it = item; it != NULL; it = it->next())
            {
                if (idx > 0)
                {
                    sprintfRecord(pos, rec, "-->");
                }
                printItem(pos, ctrl, rec, idx, it);
                idx++;
            }
            sprintfRecord(pos, rec, " ");
        }
    }
    va_list argp;
    va_start(argp, fmt);
    vsprintfRecord(pos, rec, fmt, argp);
    va_end(argp);

    // Data.
    rec->data = (unsigned char *)&rec->msg[pos + 1];
    rec->dataSize = 0;
    if (pos >= TRACE_BUFFER_MSG_SIZE - 1)
    {
        rec->dataTruncated = true;
    }
    else
    {
        rec->dataTruncated = false;

        size_t dataAvail = TRACE_BUFFER_MSG_SIZE - pos - 1;
        if (data != NULL && dataSize > 0)
        {
            if (dataSize <= dataAvail)
            {
                rec->dataSize = dataSize;
            }
            else
            {
                rec->dataTruncated = true;
                rec->dataSize = dataAvail;
            }
            rec->dataSize = dataSize < dataAvail ? dataSize : dataAvail;
            memcpy(rec->data, data, rec->dataSize);
        }
        else if (logItemData && item != NULL)
        {
            for (const AQItem *it = item; dataAvail > 0 && it != NULL; it = it->next())
            {
                size_t itemSize = it->size();
                if (itemSize > dataAvail)
                {
                    rec->dataTruncated = true;
                    itemSize = dataAvail;
                }
                memcpy(&rec->data[rec->dataSize], &(*it)[0], itemSize);
                dataAvail -= itemSize;
                rec->dataSize += itemSize;
            }
        }
    }
}


//------------------------------------------------------------------------------
void TraceBuffer::printItem(size_t& pos, aq::CtrlOverlay *ctrl,
                            Record *rec, int idx, const AQItem *item)
{
    uint32_t ctrlq = item->ctrl();
    uint32_t capacity = ctrlq & CtrlOverlay::CTRLQ_SIZE_MASK;
    uint32_t pageNum = ctrl->memToPage(item->m_mem);
    uint32_t pageCount = ctrl->sizeToPageCount(capacity);

    if (idx < 0)
    {
        sprintfRecord(pos, rec, "itm");
    }
    else
    {
        sprintfRecord(pos, rec, "it%d", idx);
    }
    sprintfRecord(pos, rec, "[%u-%u=%08X",
                  pageNum, pageNum + pageCount - 1,
                  item->queueIdentifier());
    
    uint32_t lkid = item->linkIdentifier();
    if (lkid != AQItem::QUEUE_IDENTIFIER_INVALID)
    {
        if (ctrl->options & AQ::OPTION_EXTENDABLE)
        {
            sprintfRecord(pos, rec, ":%c%c%08X",
                (lkid & AQItem::LINK_IDENTIFIER_FIRST) ? 'F' : '-',
                (lkid & AQItem::LINK_IDENTIFIER_LAST) ? 'L' : '-',
                (lkid & AQItem::QUEUE_IDENTIFIER_MASK));
        }
        else if (ctrl->options & CtrlOverlay::OPTION_HAS_LINK_IDENTIFIER)
        {
            sprintfRecord(pos, rec, ":%08X", lkid);
        }
    }

    sprintfRecord(pos, rec, ":%c%c%cQ%uL%u",
        (ctrlq & CtrlOverlay::CTRLQ_CLAIM_MASK) ? 'c' : '-',
        (ctrlq & CtrlOverlay::CTRLQ_COMMIT_MASK) ? 'C' : '-',
        (ctrlq & CtrlOverlay::CTRLQ_DISCARD_MASK) ? 'D' : '-',
        (ctrlq & CtrlOverlay::CTRLQ_SEQ_MASK) >> CtrlOverlay::REF_SEQ_SHIFT,
        capacity);

    if (item->size() != (ctrlq & CtrlOverlay::CTRLQ_SIZE_MASK))
    {
        sprintfRecord(pos, rec, ":L%u", (unsigned int)item->size());
    }
    sprintfRecord(pos, rec, "]");
}

//------------------------------------------------------------------------------
void TraceBuffer::sprintfRecord(size_t& pos, Record *rec, const char *fmt, ...)
{
    va_list argp;

    va_start(argp, fmt);
    vsprintfRecord(pos, rec, fmt, argp);
    va_end(argp);
}

//------------------------------------------------------------------------------
void TraceBuffer::vsprintfRecord(size_t& pos, Record *rec, const char *fmt, 
                                 va_list argp)
{
    if (pos < TRACE_BUFFER_MSG_SIZE - 1)
    {
        size_t avail = TRACE_BUFFER_MSG_SIZE - 1 - pos;
#ifdef WIN32
        int c = _vsnprintf(&rec->msg[pos], avail, fmt, argp);
#else
        int c = vsnprintf(&rec->msg[pos], avail, fmt, argp);
#endif
        if (c < 0 || (size_t)c >= avail)
        {
            pos = TRACE_BUFFER_MSG_SIZE - 1;
            rec->msg[TRACE_BUFFER_MSG_SIZE - 1] = '\0';
        }
        else
        {
            pos += c;
        }
    }
}

//------------------------------------------------------------------------------
void TraceBuffer::writeRecord(ostream& os, const TraceBuffer::Record& rec) const
{
    const char *func = rec.function;
    for (const char *ptr = func; *ptr != '\0'; ++ptr)
    {
        if (*ptr == ':')
        {
            func = &ptr[1];
        }
    }

    os << rec.order << " | " << setw(4) << m_id << " | ";

    if (!rec.hasCtrl)
    {
        os << "               ";
    }
    else
    {
        uint32_t headIdx = rec.headRef & CtrlOverlay::REF_INDEX_MASK;
        uint32_t tailIdx = rec.tailRef & CtrlOverlay::REF_INDEX_MASK;
        os << "Q[" << setw(5) << tailIdx << "->" << setw(5) << headIdx << "]";
    }
    os << " |" << setw(17) << func << ":" << setw(3) << rec.line;
    switch (rec.context)
    {
    case TraceBuffer::Record::Invalid:
        os << " !!! ";
        break;

    case TraceBuffer::Record::Entry:
        os << " --> ";
        break;

    case TraceBuffer::Record::EntryExit:
        os << " <-> ";
        break;

    case TraceBuffer::Record::Exit:
        os << " <-- ";
        break;

    default:
        os << "     ";
        break;
    }

    os << rec.msg;

    if (rec.dataSize > 0)
    {
        bool ascii = true;
        for (size_t i = 0; i < rec.dataSize && ascii; ++i)
        {
            ascii = rec.data[i] >= 32 && rec.data[i] <= 127;
        }
        if (ascii)
        {
            os << " \"";
            for (size_t i = 0; i < rec.dataSize && ascii; ++i)
            {
                os << (char)rec.data[i];
            }
            if (rec.dataTruncated)
            {
                os << "...";
            }
            os << "\"";
        }
        else
        {
            os << " {";
            for (size_t i = 0; i < rec.dataSize; ++i)
            {
                if (i % 4 == 0 && i != 0)
                {
                    os << " ";
                }
                os << hex << setw(2) << setfill('0') << (unsigned int)rec.data[i];
            }
            os << dec << setfill(' ');
            if (rec.dataTruncated)
            {
                os << "...";
            }
            os << "}";
        }
    }
    else if (rec.dataTruncated)
    {
        os << " ...";
    }
    os << endl;
}




}
//=============================== End of File ==================================
