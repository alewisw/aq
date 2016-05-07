//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQSnapshot.h"
#include "AQ.h"
#include "AQUnformattedException.h"

#include "Crc32.h"
#include "CtrlOverlay.h"
#include "LinkedItemProcessor.h"
#include "TestPointNotifier.h"
#include "TraceBuffer.h"

#include <algorithm>
#include <stddef.h>
#include <string.h>
#include <sstream>

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

//------------------------------------------------------------------------------
AQSnapshot::AQSnapshot(void)
    : m_trace(NULL)
    , m_mem(NULL)
    , m_memSize(0)
    , m_items(NULL)
    , m_itemCount(0)
{
}

//------------------------------------------------------------------------------
AQSnapshot::AQSnapshot(aq::TraceBuffer *trace)
    : m_trace(trace)
    , m_mem(NULL)
    , m_memSize(0)
    , m_items(NULL)
    , m_itemCount(0)
{
}

//------------------------------------------------------------------------------
AQSnapshot::AQSnapshot(const AQ& queue)
    : m_trace(NULL)
    , m_mem(NULL)
    , m_memSize(0)
    , m_items(NULL)
    , m_itemCount(0)
{
    snap(queue);
}

//------------------------------------------------------------------------------
AQSnapshot::AQSnapshot(const AQ& queue, aq::TraceBuffer *trace)
    : m_trace(trace)
    , m_mem(NULL)
    , m_memSize(0)
    , m_items(NULL)
    , m_itemCount(0)
{
    snap(queue);
}

//------------------------------------------------------------------------------
AQSnapshot::AQSnapshot(const AQSnapshot& other)
    : m_mem(NULL)
    , m_memSize(0)
    , m_items(NULL)
    , m_itemCount(0)
{
    clone(other);
}

//------------------------------------------------------------------------------
AQSnapshot& AQSnapshot::operator=(const AQSnapshot& other)
{
    if (this != &other)
    {
        reset();
        clone(other);
    }
    return *this;
}

//------------------------------------------------------------------------------
AQSnapshot::~AQSnapshot(void)
{
    reset();
}

//------------------------------------------------------------------------------
void AQSnapshot::snap(const AQ& queue)
{
    snap1InitialHead(queue);
    snap2InitialCtrlq();
    snap3PageMemory();
    snap4FinalHead();
}

//------------------------------------------------------------------------------
void AQSnapshot::snap1InitialHead(const AQ& queue)
{
    reset();

    const void *mem = (const void *)queue.m_ctrl;
    size_t memSize = queue.memorySize();

    if (mem == NULL || memSize < offsetof(CtrlOverlay, ctrlq))
    {
        ostringstream ss;
        ss << "Cannot take a MpscSnapshot of NULL or empty memory";
        throw AQUnformattedException(ss.str());
    }

    // Get the control overlay.
    CtrlOverlay *c = (CtrlOverlay *)mem;
    if (!c->isFormatted(memSize))
    {
        ostringstream ss;
        ss << "Cannot take a MpscSnapshot as it does not contain a valid MpscShmQueue";
        throw AQUnformattedException(ss.str());
    }
    m_srcCtrl = c;

    // This is the raw destination memory.
    m_mem = new unsigned char[memSize];
    m_memSize = memSize;
    CtrlOverlay *dstCtrl = (CtrlOverlay *)m_mem;

    // Copy over the control structure.
    memcpy(dstCtrl, m_srcCtrl, offsetof(CtrlOverlay, headRef));

    m_initHeadRef = Atomic::read(&m_srcCtrl->headRef);
    TRACE_ENTRY("head-init<" TRACE_REF_FMT ">", TRACE_REF(m_initHeadRef));
}

//------------------------------------------------------------------------------
void AQSnapshot::snap2InitialCtrlq(void)
{
    CtrlOverlay *dstCtrl = (CtrlOverlay *)m_mem;

    uint32_t mul = 1;
    if (dstCtrl->options & AQ::OPTION_CRC32)
    {
        mul++;
    }
    if (dstCtrl->options & CtrlOverlay::OPTION_HAS_LINK_IDENTIFIER)
    {
        mul++;
    }

    for (uint32_t i = 0; i < m_srcCtrl->pageCount * mul; ++i)
    {
        dstCtrl->ctrlq[i] = Atomic::read(&m_srcCtrl->ctrlq[i]);
    }
    TRACE("ctrlq-init");
}

//------------------------------------------------------------------------------
void AQSnapshot::snap3PageMemory(void)
{
    CtrlOverlay *dstCtrl = (CtrlOverlay *)m_mem;

    memcpy(dstCtrl->pageToMem(0), m_srcCtrl->pageToMem(0), m_srcCtrl->pageCount << m_srcCtrl->pageSizeShift);
    TRACE("page-mem");
}

//------------------------------------------------------------------------------
void AQSnapshot::snap4FinalHead(void)
{
    LinkedItemProcessor linkProcessor;

    CtrlOverlay *dstCtrl = (CtrlOverlay *)m_mem;

    // Finally grab the head at the end of the snapshot process.
    m_finalHeadRef = Atomic::read(&m_srcCtrl->headRef);
    TRACE("head-final<" TRACE_REF_FMT ">", TRACE_REF(m_finalHeadRef));

    // Allocate enough memory to hold all possible items.
    m_items = new AQItem[dstCtrl->pageCount];
    m_itemCount = 0;

    // During the copy of the control queue and data the head and tail can be
    // asynchronously modified.  We only care about the head position when
    // taking a snapshot because the change it head will indicate which
    // items are suspect in the snapshot data.  Consider the following
    // initial scenario:
    //
    //              tail            head
    //              |(7)            |(7)
    //              V               V
    //        +---+---+---+---+---+---+---+---+ 
    // ctrlq  |*0*|#1#|#2#|#3#|#4#|*5*|*6*|*7*| <--- 1 word per page
    //        +---+---+---+---+---+---+---+---+
    //                             ^ ^
    //                          (7)| |(6)
    //                      snapHead snapTail
    //
    // The '#' shows unconsumed items in the queue, while the '*' shows item
    // that have already been consumed.
    //
    // The key factor here is the change in head; we cannot trust any entry in the
    // range [head, finalHead); consider the following two cases.
    //
    // (1)          tail            head   finalHead
    //              |(7)            |(7)   |(7)
    //              V               V      V
    //        +---+---+---+---+---+---+---+---+ 
    // ctrlq  |*0*|#1#|#2#|#3#|#4#|?5?|?6?|*7*| <--- 1 word per page
    //        +---+---+---+---+---+---+---+---+
    //                              ^       ^
    //                           (7)|       |(6)
    //                       snapHead       snapTail
    //
    //
    // (2)          tail   finalHead head
    //              |      |(8)      |(7)
    //              V      V         V   
    //        +---+---+---+---+---+---+---+---+ 
    // ctrlq  |?0?|?1?|?2?|#3#|#4#|?5?|?6?|?7?| <--- 1 word per page
    //        +---+---+---+---+---+---+---+---+
    //                      ^       ^
    //                   (7)|       |(7)       
    //               snapTail       snapHead
    //
    // In case (1) the head has moved to consume space that was previously 'free'.
    // In case (2) the head has overtaken the original tail position.
    // There is a third case; that is when the head has overtaken itself.  In that
    // case there are no items in the snapshot!
    uint32_t initHeadIdx = m_initHeadRef & CtrlOverlay::REF_INDEX_MASK;
    uint32_t finalHeadIdx = m_finalHeadRef & CtrlOverlay::REF_INDEX_MASK;
    uint32_t initHeadSeq = m_initHeadRef & CtrlOverlay::REF_SEQ_MASK;
    uint32_t finalHeadSeq = m_finalHeadRef & CtrlOverlay::REF_SEQ_MASK;

    uint32_t tailRef;
    uint32_t headRef;
    if (finalHeadIdx >= initHeadIdx)
    {
        if (initHeadSeq == finalHeadSeq)
        {
            // Case (1): The head simply advanced; the initial head represents
            //           the start of the invalid section while the final head 
            //           represents the first valid entry.
            tailRef = finalHeadIdx | ((finalHeadSeq - CtrlOverlay::REF_SEQ_INCR) & CtrlOverlay::REF_SEQ_MASK);
            headRef = m_initHeadRef;
        }
        else
        {
            // Case (3): No valid entries exist in the queue at all, it has 
            //           wrapped at least once.
            tailRef = m_finalHeadRef;
            headRef = m_finalHeadRef;
        }
    }
    else
    {
        if (initHeadSeq + CtrlOverlay::REF_SEQ_INCR == finalHeadSeq)
        {
            // Case (2): The head has wrapped around around; the initial head 
            //           is the start of the invalid section while the final 
            //           head is the first valid entry.
            tailRef = finalHeadIdx | ((finalHeadSeq - CtrlOverlay::REF_SEQ_INCR) & CtrlOverlay::REF_SEQ_MASK);
            headRef = m_initHeadRef;
        }
        else
        {
            // Case (3): No valid entries exist in the queue at all - it has wrapped
            //           more than once.
            tailRef = m_finalHeadRef;
            headRef = m_finalHeadRef;
        }
    }
    TRACE("tail<" TRACE_REF_FMT "> head<" TRACE_REF_FMT ">",
        TRACE_REF(tailRef), TRACE_REF(headRef));
    dstCtrl->headRef = headRef;
    dstCtrl->tailRef = tailRef;


    // Memory has been allocated and pointers setup correctly.  Now build the
    // item array by walking from the tail, advancing it until it is equal
    // to the head.
    while (tailRef != headRef)
    {
        uint32_t tail = dstCtrl->queueRefToIndex(tailRef);
        uint32_t ctrlTail = Atomic::read(&dstCtrl->ctrlq[tail]);
        uint32_t ctrlSize = ctrlTail & CtrlOverlay::CTRLQ_SIZE_MASK;
        uint32_t ctrlFlags = ctrlTail & CtrlOverlay::CTRLQ_FLAGS_MASK;
        uint32_t advance = dstCtrl->sizeToPageCount(ctrlSize);
        if (advance == 0)
        {
            advance = 1;
        }

        if (((ctrlTail & CtrlOverlay::CTRLQ_SEQ_MASK) == (tailRef & CtrlOverlay::CTRLQ_SEQ_MASK))
            && ctrlSize > 0
            && (ctrlFlags != CtrlOverlay::CTRLQ_DISCARD_MASK)
            && (ctrlFlags != (CtrlOverlay::CTRLQ_COMMIT_MASK | CtrlOverlay::CTRLQ_DISCARD_MASK)))
        {
            AQItem& item = m_items[m_itemCount];

            item.m_ctrl = ctrlTail;
            item.m_memSize = ctrlSize;
            item.m_mem = dstCtrl->pageToMem(tail);
            item.m_quid = tailRef & AQItem::QUEUE_IDENTIFIER_MASK;

            uint32_t extPageNum = tail;
            if (dstCtrl->options & CtrlOverlay::OPTION_HAS_LINK_IDENTIFIER)
            {
                extPageNum += dstCtrl->pageCount;
                item.m_lkid = dstCtrl->ctrlq[extPageNum];
            }
            else
            {
                item.m_lkid = AQItem::QUEUE_IDENTIFIER_INVALID;
            }

            if (dstCtrl->options & AQ::OPTION_CRC32)
            {
                extPageNum += dstCtrl->pageCount;

                uint32_t crc = CalculateItemCrc32(item, dstCtrl->options);
                item.m_checksumValid = crc == dstCtrl->ctrlq[extPageNum];
                if (item.m_checksumValid)
                {
                    TRACE_1ITEMDATA(dstCtrl, &item, "crc[%08X]", crc);
                }
                else
                {
                    TRACE_1ITEMDATA(dstCtrl, &item, "crc[%08X] ERROR expect[%08X]", crc, dstCtrl->ctrlq[extPageNum]);
                }
            }
            else
            {
                item.m_checksumValid = true;
                TRACE_1ITEMDATA(dstCtrl, &item);
            }
            if (dstCtrl->options & AQ::OPTION_EXTENDABLE)
            {
                if (linkProcessor.nextItem(item) == LinkedItemProcessor::PRODUCED)
                {
                    TRACE_ITEM(dstCtrl, &item, "extendable item linked");
                    m_itemCount++;
                }
            }
            else
            {
                m_itemCount++;
            }
        }
        // We can only skip the 'advance' count when it would lead us to the very end
        // of the buffer if this is wasted space.  It is only wasted space when:
        //  - The SEQ field is not an exact inverse of the expected value.
        //  - The ctrlFlags is DISCARD or COMMIT|DISCARD (wasted space).
        //  - The item size must be an exactly multiple of the page size.
        //  - The current page plus the advance count leads to the page count.
        else if (((ctrlTail & CtrlOverlay::CTRLQ_SEQ_MASK) != (tailRef & CtrlOverlay::CTRLQ_SEQ_MASK))
            || (ctrlFlags != CtrlOverlay::CTRLQ_DISCARD_MASK && ctrlFlags != (CtrlOverlay::CTRLQ_COMMIT_MASK | CtrlOverlay::CTRLQ_DISCARD_MASK))
            || (ctrlSize != (advance << dstCtrl->pageSizeShift))
            || (tail + advance != dstCtrl->pageCount))
        {
            // This is a garbage item of some sort; just move to the next position.
            advance = 1;
            TRACE("ignore pg<" TRACE_PGS_FMT "> ctrl<" TRACE_CTQ_FMT ">", TRACE_PGS(tailRef, advance), TRACE_CTQ(ctrlTail));
        }
        else
        {
            TRACE("ignore-waste pg<" TRACE_PGS_FMT "> ctrl<" TRACE_CTQ_FMT ">", TRACE_PGS(tailRef, advance), TRACE_CTQ(ctrlTail));
        }
        tailRef = dstCtrl->queueRefIncrement(tailRef, advance);
    }

    TRACE("Found total of %u items", (unsigned int)size());
    for (size_t i = 0; i < size(); ++i)
    {
        TRACE_ITEM(dstCtrl, &(*this)[i]);
    }

    TRACE_EXIT();
}

//------------------------------------------------------------------------------
void AQSnapshot::reset(void)
{
    if (m_mem != NULL)
    {
        delete[] m_mem;
        m_mem = NULL;
    }
    m_memSize = 0;
    delete[] m_items;
    m_items = NULL;
    m_itemCount = 0;
    m_initHeadRef = 0;
    m_finalHeadRef = 0;
    m_srcCtrl = NULL;
}

//------------------------------------------------------------------------------
void AQSnapshot::clone(const AQSnapshot& other)
{
    m_memSize = other.m_memSize;
    m_mem = new unsigned char[m_memSize];
    memcpy(m_mem, other.m_mem, m_memSize);

    if (other.m_itemCount > 0)
    {
        m_itemCount = other.m_itemCount;
        m_items = new AQItem[m_itemCount];
        for (size_t i = 0; i < m_itemCount; ++i)
        {
            ptrdiff_t memIdx = (intptr_t)other.m_items[i].m_mem - (intptr_t)other.m_mem;

            m_items[i] = other.m_items[i];
            m_items[i].m_mem = &m_mem[memIdx];
        }
    }
}




//=============================== End of File ==================================
