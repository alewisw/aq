//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "ItemGenerator.h"

#include <stdexcept>

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
ItemGenerator::ItemGenerator(int threadNum, size_t pageSize, const vector<size_t> &itemPages)
    : m_threadNum(threadNum)
    , m_pageSize(pageSize)
    , m_itemPages(itemPages)
    , m_count(0)
    , m_prng(threadNum)
    , m_itemMaxSize(0)
    , m_item(NULL)
    , m_randBufSize(0)
    , m_randBuf(NULL)
{
    if (m_threadNum > 99999)
    {
        throw logic_error("Thread number must be between 0 and 99999");
    }

    // Calculate the maximum item size.
    for (size_t i = 0; i < m_itemPages.size(); ++i)
    {
        size_t recSize = m_itemPages[i] * m_pageSize;
        if (recSize > m_itemMaxSize)
        {
            m_itemMaxSize = recSize;
        }
    }

    // Allocate the item memory.
    m_item = new unsigned char[m_itemMaxSize + 1];

    // Allocate the random data buffer and create the random data.
    m_randBufSize = m_itemMaxSize * 10;
    m_randBuf = new unsigned char[m_randBufSize];
    for (size_t i = 0; i < m_randBufSize; ++i)
    {
        m_randBuf[i] = 'a' + (char)(m_prng.next() % 26);
    }
}

//------------------------------------------------------------------------------
ItemGenerator::ItemGenerator(const ItemGenerator& other)
    : m_threadNum(other.m_threadNum)
    , m_pageSize(other.m_pageSize)
    , m_itemPages(other.m_itemPages)
    , m_count(other.m_count)
    , m_prng(other.m_prng)
    , m_itemMaxSize(other.m_itemMaxSize)
    , m_item(new unsigned char[m_itemMaxSize])
    , m_randBufSize(other.m_randBufSize)
    , m_randBuf(new unsigned char[m_randBufSize])
{
    memcpy(m_randBuf, other.m_randBuf, m_randBufSize);
}

//------------------------------------------------------------------------------
ItemGenerator& ItemGenerator::operator=(const ItemGenerator& other)
{
    if (this != &other)
    {
        m_threadNum = other.m_threadNum;
        m_pageSize = other.m_pageSize;
        m_itemPages = other.m_itemPages;
        m_count = other.m_count;
        m_prng = other.m_prng;
        m_itemMaxSize = other.m_itemMaxSize;
        m_item = new unsigned char[m_itemMaxSize];
        m_randBufSize = other.m_randBufSize;
        m_randBuf = new unsigned char[m_randBufSize];
        memcpy(m_randBuf, other.m_randBuf, m_randBufSize);
    }
    return *this;
}

//------------------------------------------------------------------------------
ItemGenerator::~ItemGenerator(void)
{
    delete[] m_randBuf;
    delete[] m_item;
}

//------------------------------------------------------------------------------
const unsigned char *ItemGenerator::get(size_t& memSize, uint32_t& linkId) const
{
    unsigned int r = m_prng.get();

    // Use the random number as the link identifier.
    linkId = r;

    // Generate three random numbers based on the PRNG
    size_t x = r / m_itemPages.size();
    size_t y = r % m_itemPages.size();
    size_t z = x % (m_pageSize - 1);
    x = (x / (m_pageSize - 1));
    size_t pageCount = m_itemPages[y];

    // Calculate the size adjustment.
    memSize = (pageCount - 1) * m_pageSize + 1 + z;
    if (memSize < RECORD_MIN_LEN)
    {
        memSize = RECORD_MIN_LEN;
    }

    // Construct the header.
    int len = sprintf((char *)m_item, RECORD_FORMAT, memSize, m_threadNum, m_count);
    if (len != RECORD_MIN_LEN - 1)
    {
        throw logic_error(string("Length invalid"));
    }

    // Find a random starting point the random buffer then change the buffer.
    x = x % (m_randBufSize - (memSize - RECORD_MIN_LEN + 1));

    // Copy in the random data.
    memcpy(&m_item[len], &m_randBuf[x], memSize - RECORD_MIN_LEN + 1);
    m_item[memSize] = '\0';

    return m_item;
}

//------------------------------------------------------------------------------
void ItemGenerator::next(void)
{
    m_count++;

    unsigned int x = m_prng.next() % m_randBufSize;
    if (m_randBuf[x] == 'z')
    {
        m_randBuf[x] = 'a';
    }
    else
    {
        m_randBuf[x]++;
    }
}

//------------------------------------------------------------------------------
const unsigned char *ItemGenerator::next(size_t& memSize, uint32_t& linkId)
{
    const unsigned char *rec = get(memSize, linkId);
    next();
    return rec;
}



//=============================== End of File ==================================
