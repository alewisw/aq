//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQStrawMan.h"

#include "AQItem.h"
#include "AQWriterItem.h"




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
AQStrawManBase::AQStrawManBase(int pageSizeShift, size_t pageCount)
    : m_pageCount(pageCount)
    , m_pageSizeShift(pageSizeShift)
{
    size_t pageSize = 1 << pageSizeShift;

    m_mem = new unsigned char[pageCount * offsetof(Page, m_mem) + (pageCount << pageSizeShift)];
}

//------------------------------------------------------------------------------
AQStrawManBase::~AQStrawManBase(void)
{
    delete[] m_mem;
}

//------------------------------------------------------------------------------
void AQStrawManBase::reset(void)
{
    committedLock();
    m_committed.clear();
    committedUnlock();

    freeLock();
    m_free.clear();
    size_t off = 0;
    for (size_t i = 0; i < m_pageCount; ++i)
    {
        m_free.push_back((Page *)&m_mem[off]);
        off += 1 << m_pageSizeShift;
        off += offsetof(Page, m_mem);
    }
    freeUnlock();
}

//------------------------------------------------------------------------------
AQStrawManBase::Page *AQStrawManBase::memToPage(unsigned char *mem) const
{
    return (Page *)&mem[-(int)offsetof(Page, m_mem)];
}

//------------------------------------------------------------------------------
bool AQStrawManBase::claim(AQWriterItem& item, size_t memSize)
{
    freeLock();
    if (m_free.size() == 0)
    {
        freeUnlock();
        item.clear();
        return false;
    }
    else
    {
        Page *page = m_free.front();
        m_free.pop_front();
        freeUnlock();

        page->m_memSize = memSize;
        item.m_mem = &page->m_mem[0];
        item.m_memSize = page->m_memSize;
        return true;
    }
}

//------------------------------------------------------------------------------
bool AQStrawManBase::commit(AQWriterItem& item)
{
    if (item.isAllocated())
    {
        committedLock();
        m_committed.push_back(memToPage(item.m_mem));
        committedUnlock();
        item.clear();
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
bool AQStrawManBase::retrieve(AQItem& item)
{
    committedLock();
    if (m_committed.size() == 0)
    {
        committedUnlock();
        item.clear();
        return false;
    }
    else
    {
        Page *mem = m_committed.front();
        m_committed.pop_front();
        committedUnlock();

        item.m_mem = mem->m_mem;
        item.m_memSize = mem->m_memSize;
        return true;
    }
}

//------------------------------------------------------------------------------
void AQStrawManBase::release(AQItem& item)
{
    if (item.isAllocated())
    {
        freeLock();
        m_free.push_back(memToPage(item.m_mem));
        freeUnlock();
        item.clear();
    }
}




//=============================== End of File ==================================
