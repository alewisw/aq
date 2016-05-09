//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQLogStringBuilder.h"

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
AQLogStringBuilder::AQLogStringBuilder(void)
    : m_scopyPos(0)
    , m_dcopy(NULL)
    , m_totalSize(0)
{
}

//------------------------------------------------------------------------------
AQLogStringBuilder::~AQLogStringBuilder(void)
{
    if (m_dcopy != NULL)
    {
        for (size_t i = 0; i < m_dcopy->size(); ++i)
        {
            free((*m_dcopy)[i]);
        }
        delete m_dcopy;
    }
}

//------------------------------------------------------------------------------
void AQLogStringBuilder::clear(void)
{
    // Clear the vector.
    m_vect.clear();
    m_totalSize = 0;

    // Clear the copied strings.
    m_scopyPos = 0;
    if (m_dcopy != NULL)
    {
        for (size_t i = 0; i < m_dcopy->size(); ++i)
        {
            free((*m_dcopy)[i]);
        }
        m_dcopy->clear();
    }
}

//------------------------------------------------------------------------------
char *AQLogStringBuilder::insertEmpty(iterator& pos, size_t len)
{
    if (len == 0)
    {
        return NULL;
    }
    char *str = allocString(len);
    insertPointer(pos, str, len);
    return str;
}

//------------------------------------------------------------------------------
void AQLogStringBuilder::insertCopy(iterator& pos, const char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return;
    }

    char *strCpy = insertEmpty(pos, len);
    memcpy(strCpy, str, len);
}

//------------------------------------------------------------------------------
void AQLogStringBuilder::insertFree(iterator& pos, char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return;
    }

    if (m_dcopy == NULL)
    {
        m_dcopy = new vector<char *>();
    }
    m_dcopy->push_back(str);
    insertPointer(pos, str, len);
}

//------------------------------------------------------------------------------
void AQLogStringBuilder::insertPointer(iterator& pos, const char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return;
    }

    // Split the queue at the iterator position; this does nothing in the case
    // where the iterator is already at the start of a vector.
    split(pos);
    struct iovec iov = { (void *)str, len };

    // Insert the new item.
    m_vect.insert(m_vect.begin() + pos.m_vectIdx, iov);
    m_totalSize += len;
}

//------------------------------------------------------------------------------
void AQLogStringBuilder::erase(iterator& pos, size_t len)
{
    // Split at our erase position so ensure we are at the start of a 
    // vector.
    split(pos);

    while (len > 0 && pos != end())
    {
        struct iovec& iov = m_vect[pos.m_vectIdx];
        if (len < iov.iov_len)
        {
            // Just erasing part of this vector.
            iov.iov_len -= len;
            iov.iov_base = (char *)iov.iov_base + len;
            m_totalSize -= len;
            len = 0;
        }
        else
        {
            // Must erase this entire vector.
            m_totalSize -= iov.iov_len;
            len -= iov.iov_len;
            m_vect.erase(m_vect.begin() + pos.m_vectIdx);
        }
    }
}

//------------------------------------------------------------------------------
bool AQLogStringBuilder::find(iterator& pos, char ch)
{
    const iterator e = end();
    while (pos != e && *pos != ch)
    {
        pos++;
    }
    return pos != e;
}

//------------------------------------------------------------------------------
bool AQLogStringBuilder::find(iterator& pos, const char *str)
{
    // Get the length; refuse to match an empty string - move the iterator to
    // the end and return false.
    const iterator e = end();
    if (str != NULL && str[0] != '\0')
    {
        // Advance our position - we will compare in reverse 
        size_t len = strlen(str);
        const iterator searchEnd = e - (len - 1);
        while (pos < searchEnd)
        {
            if (*pos == str[0])
            {
                const struct iovec &iov = m_vect[pos.m_vectIdx];
                if (pos.m_vectOff + len <= iov.iov_len)
                {
                    // Can do a fast compare with memcmp().
                    if (memcmp(((char *)iov.iov_base) + pos.m_vectOff + 1, &str[1], len - 1) == 0)
                    {
                        // Found the item; return it.
                        return true;
                    }
                }
                else
                {
                    // Crosses boundaries - need to do it the old way.
                    iterator it = pos + 1;
                    size_t i = 1;
                    while (i < len && *it++ == str[i])
                    {
                        ++i;
                    }
                    if (i == len)
                    {
                        // Found the item - return it.
                        return true;
                    }
                }
            }
            ++pos;
        }
    }

    // Reaching here indicates failure.
    pos = e;
    return false;
}

//------------------------------------------------------------------------------
void AQLogStringBuilder::toCharArray(char *dst, size_t dstLen) const
{
    size_t vectIdx = 0;
    size_t dstPos = 0;
    while (vectIdx < m_vect.size() && dstPos < dstLen)
    {
        const struct iovec& iov = m_vect[vectIdx++];

        size_t cpyLen = dstLen - dstPos;
        if (iov.iov_len < cpyLen)
        {
            cpyLen = iov.iov_len;
        }
        memcpy(&dst[dstPos], iov.iov_base, cpyLen);
        dstPos += cpyLen;
    }

    if (dstPos < dstLen)
    {
        dst[dstPos] = '\0';
    }
}

//------------------------------------------------------------------------------
string AQLogStringBuilder::toString(void) const
{
    size_t strBufLen = size() + 1;
    char *strBuf = new char[strBufLen];
    toCharArray(strBuf, strBufLen);
    string s = string(strBuf);
    delete[] strBuf;
    return s;
}

//------------------------------------------------------------------------------
void AQLogStringBuilder::split(iterator& pos)
{
    if (pos.m_vectOff != 0)
    {
        struct iovec& iovc = m_vect[pos.m_vectIdx];
        struct iovec iov =
        {
            &((char *)iovc.iov_base)[pos.m_vectOff],
            iovc.iov_len - pos.m_vectOff
        };
        iovc.iov_len = pos.m_vectOff;

        pos.m_vectIdx++;
        pos.m_vectOff = 0;
        m_vect.insert(m_vect.begin() + pos.m_vectIdx, iov);
    }
}

//------------------------------------------------------------------------------
char *AQLogStringBuilder::allocString(size_t len)
{
    char *str;

    if (len <= SCOPY_SIZE - m_scopyPos)
    {
        str = &m_scopy[m_scopyPos];
        m_scopyPos += len;
    }
    else
    {
        if (m_dcopy == NULL)
        {
            m_dcopy = new vector<char *>();
        }
        str = new char[len];
        m_dcopy->push_back(str);
    }
    return str;
}



//=============================== End of File ==================================
