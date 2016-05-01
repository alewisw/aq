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

#include "CtrlOverlay.h"

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

// Local storage in case the constant is taken as a reference.
#ifdef __GNUC__
const uint32_t AQItem::QUEUE_IDENTIFIER_MASK;
const uint32_t AQItem::QUEUE_IDENTIFIER_USER_MASK;
const uint32_t AQItem::QUEUE_IDENTIFIER_USER_BIT;
const uint32_t AQItem::QUEUE_IDENTIFIER_INVALID;
#endif




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void AQItem::cloneFrom(const AQItem& other)
{
    // AQItem lists work in the following way:
    //  - the memory for the first item is under application control.
    //  - all items after the first are owned by the first; when the first is
    //    deleted they are deleted.
    // Thus we need to handle two different situations:
    //
    //  (1) this == first --> In this case we can simply delete all the objects
    //                        after 'first' and replace them with new instances
    //                        copied from 'other'.
    //  (2) this != first --> In this case we need to delete m_next onwards, 
    //                        then replace them with new instance from other.
    //                        However we populate the 'first' with the 'first'
    //                        that already exists.
    //
    // As can be seen these devolve to the same thing; we:
    //  (1) walk the existing chain overwriting the content.
    //  (2) create a new object if none exist.
    //  (3) 'delete' extra objects from the list.


    // Copy 'src' to 'dst'.
    const AQItem *src = &other;
    AQItem *dst = this;
    while (src != NULL)
    {
        dst->m_mem = src->m_mem;
        dst->m_memSize = src->m_memSize;
        dst->m_ctrl = src->m_ctrl;
        dst->m_checksumValid = src->m_checksumValid;
        dst->m_quid = src->m_quid;
        dst->m_lkid = src->m_lkid;

        // Move to the next object, creating if we have to.
        src = src->next();
        if (src != NULL && dst->m_next == NULL)
        {
            AQItem *item = newInstance();
            item->m_first = m_first;
            dst->m_next = item;
            item->m_prev = dst;
            m_first->m_prev = item;
        }
        dst = dst->m_next;
    }

    // Delete anything that is no longer required.
    if (dst != NULL)
    {
        // Make sure we correctly point to the last item in the list.
        m_first->m_prev = dst->m_prev;
        dst->m_prev->m_next = NULL;
        while (dst != NULL)
        {
            AQItem *delNext = dst->m_next;
            delete dst;
            dst = delNext;
        }
    }
}

//------------------------------------------------------------------------------
void AQItem::clearList()
{
    if (m_first == this)
    {
        AQItem *next = m_next;
        while (next != NULL)
        {
            AQItem *curr = next;
            next = curr->m_next;
            delete curr;
        }
        m_next = NULL;
        m_prev = this;
    }
}

//------------------------------------------------------------------------------
size_t AQItem::capacity(void) const
{
    return ctrl() & CtrlOverlay::CTRLQ_SIZE_MASK;
}

//------------------------------------------------------------------------------
bool AQItem::isCommitted(void) const
{ 
    return !!(ctrl() & CtrlOverlay::CTRLQ_COMMIT_MASK); 
}

//------------------------------------------------------------------------------
bool AQItem::isReleased(void) const
{
    return (ctrl() & (CtrlOverlay::CTRLQ_CLAIM_MASK | CtrlOverlay::CTRLQ_DISCARD_MASK)) != CtrlOverlay::CTRLQ_CLAIM_MASK;
}




//=============================== End of File ==================================
