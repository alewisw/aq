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

// Require at least this many bytes in the static buffer to try printing into
// it strftime().
#define STRFTIME_SCOPY_MIN_SIZE         20

// The base size of the memory to allocated for dynamic strftime generation.
#define STRFTIME_DCOPY_BASE_SIZE        512




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
    : m_options(OPTION_COALESCE_ADJACENT_IOV)
    , m_scopyPos(0)
    , m_dcopy(NULL)
    , m_dcopyFree(NULL)
    , m_dcopyFreeSize(0)
    , m_totalSize(0)
{
}

//------------------------------------------------------------------------------
AQLogStringBuilder::AQLogStringBuilder(uint32_t options)
    : m_options(options)
    , m_scopyPos(0)
    , m_dcopy(NULL)
    , m_dcopyFree(NULL)
    , m_dcopyFreeSize(0)
    , m_totalSize(0)
{
}

//------------------------------------------------------------------------------
AQLogStringBuilder::AQLogStringBuilder(const AQLogStringBuilder& other)
    : m_options(other.m_options)
    , m_scopyPos(0)
    , m_dcopy(NULL)
    , m_dcopyFree(NULL)
    , m_dcopyFreeSize(0)
    , m_totalSize(0)
{
    appendCopy(other);
}

//------------------------------------------------------------------------------
AQLogStringBuilder &AQLogStringBuilder::operator=(const AQLogStringBuilder& other)
{
    if (this != &other)
    {
        m_options = other.m_options;
        clear().appendCopy(other);
    }
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder::~AQLogStringBuilder(void)
{
    clear();
    if (m_dcopy != NULL)
    {
        delete m_dcopy;
    }
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::clear(void)
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
    m_dcopyFree = NULL;
    m_dcopyFreeSize = 0;
    return *this;
}

//------------------------------------------------------------------------------
char *AQLogStringBuilder::appendEmpty(size_t len)
{
    if (len == 0)
    {
        return NULL;
    }
    char *str = allocString(len);
    appendPointer(str, len);
    return str;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::appendCopy(const char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return *this;
    }

    char *strCpy = appendEmpty(len);
    memcpy(strCpy, str, len);
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::appendFree(char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return *this;
    }

    if (m_dcopy == NULL)
    {
        m_dcopy = new vector<char *>();
    }
    m_dcopy->push_back(str);
    appendPointer(str, len);
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::appendPointer(const char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return *this;
    }

    m_totalSize += len;

    // First check for a coalesce opportunity.
    if (m_vect.size() > 0)
    {
        struct iovec& iov = m_vect[m_vect.size() - 1];
        if ((char *)iov.iov_base + iov.iov_len == str)
        {
            if (m_options & OPTION_COALESCE_ADJACENT_IOV)
            {
                // Coalesce the buffers instead of making a new iov entry.
                iov.iov_len += len;
                return *this;
            }
        }
    }

    // Not possible to coalesce so add a new entry.
    struct iovec iov = { (void *)str, len };
    m_vect.push_back(iov);
    return *this;
}


//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::appendftime(const char *fmt, const struct tm *tm)
{
    size_t len = 0;
    const char *str = allocStrftime(fmt, tm, len);
    if (str != NULL && len > 0)
    {
        appendPointer(str, len);
    }
    return *this;
}

//------------------------------------------------------------------------------
char *AQLogStringBuilder::insertEmpty(const iterator& pos, size_t len)
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
AQLogStringBuilder& AQLogStringBuilder::insertCopy(const iterator& pos,
    const char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return *this;
    }

    char *strCpy = insertEmpty(pos, len);
    memcpy(strCpy, str, len);
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::insertCopy(const iterator& pos,
    const iterator& sbBegin, const iterator& sbEnd)
{
    if (sbBegin >= sbEnd)
    {
        return *this;
    }

    // Split the queue at the iterator position; this does nothing in the case
    // where the iterator is already at the start of a vector.
    size_t len = sbEnd - sbBegin;
    char *str = insertEmpty(pos, len);

    iterator it = sbBegin;
    while (it < sbEnd)
    {
        const struct iovec iovSrc = it.vect();

        size_t strLen;
        if (it.m_vectIdx == sbEnd.m_vectIdx)
        {
            // Ends within this vector.
            strLen = sbEnd.m_vectOff - it.m_vectOff;
        }
        else
        {
            // Does not end within this vector.
            strLen = iovSrc.iov_len - it.m_vectOff;
        }
        memcpy(str, (char *)iovSrc.iov_base + it.m_vectOff, strLen);
        it += strLen;
        str += strLen;
    }

    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::insertFree(const iterator& pos, char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return *this;
    }

    if (m_dcopy == NULL)
    {
        m_dcopy = new vector<char *>();
    }
    m_dcopy->push_back(str);
    insertPointer(pos, str, len);
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::insertPointer(const iterator& pos, const char *str, size_t len)
{
    if (len == 0 || str == NULL)
    {
        return *this;
    }

    // Split the queue at the iterator position; this does nothing in the case
    // where the iterator is already at the start of a vector.
    split(pos);

    m_totalSize += len;

    // Check for a coalesce opportunity.  Note that we don't attempt
    // to perform joins where a 'missing piece' is inserted as:
    //  a) This seems unlikely to occur.
    //  b) The point of coalescing is to reduce the amount of 
    //     vector shuffling in the array and joing would reduce
    //     number of iov in the array by 1 causing a shuffle.
    if (pos.m_vectIdx > 0)
    {
        // Possible coalesce backwards.
        struct iovec& iovPrev = m_vect[pos.m_vectIdx - 1];
        if ((char *)iovPrev.iov_base + iovPrev.iov_len == str)
        {
            if (m_options & OPTION_COALESCE_ADJACENT_IOV)
            {
                // Success: coalesce backwards.
                iovPrev.iov_len += len;
                return *this;
            }
        }
    }

    if (pos.m_vectIdx < m_vect.size())
    {
        // Possible coalesce forwards.
        struct iovec& iovNext = m_vect[pos.m_vectIdx];
        if (iovNext.iov_base == str + len)
        {
            if (m_options & OPTION_COALESCE_ADJACENT_IOV)
            {
                // Success: coalesce forwards.
                iovNext.iov_base = (void *)str;
                iovNext.iov_len += len;
                return *this;
            }
        }
    }

    // Coalesce not possible - just insert the new item.
    struct iovec iov = { (void *)str, len };
    m_vect.insert(m_vect.begin() + pos.m_vectIdx, iov);
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::insertPointer(const iterator& pos, const iterator& sbBegin,
    const iterator& sbEnd)
{
    if (sbBegin >= sbEnd)
    {
        return *this;
    }

    // Split the queue at the iterator position; this does nothing in the case
    // where the iterator is already at the start of a vector.
    split(pos);
    iterator it = sbBegin;
    size_t insertIdx = pos.m_vectIdx;
    while (it < sbEnd)
    {
        const struct iovec iovSrc = it.vect();

        if (it.m_vectIdx == sbEnd.m_vectIdx)
        {
            // Ends within this vector.
            struct iovec iov = { (char *)iovSrc.iov_base + it.m_vectOff, sbEnd.m_vectOff - it.m_vectOff };
            m_vect.insert(m_vect.begin() + insertIdx, iov);
            it.m_vectOff = sbEnd.m_vectOff;
            m_totalSize += iov.iov_len;
        }
        else if (it.m_vectOff == 0)
        {
            // Does not end within this vector - and requires the entire vector.
            m_vect.insert(m_vect.begin() + insertIdx, iovSrc);
            it.m_vectIdx++;
            m_totalSize += iovSrc.iov_len;
        }
        else
        {
            // Does not end within this vector and requires only part of the vector.
            struct iovec iov = { (char *)iovSrc.iov_base + it.m_vectOff, iovSrc.iov_len - it.m_vectOff };
            m_vect.insert(m_vect.begin() + insertIdx, iov);
            it.m_vectIdx++;
            it.m_vectOff = 0;
            m_totalSize += iov.iov_len;
        }
        insertIdx++;
    }
    return *this;
}


//------------------------------------------------------------------------------
size_t AQLogStringBuilder::insertftime(const iterator& pos, const char *fmt,
    const struct tm *tm)
{
    size_t len = 0;
    const char *str = allocStrftime(fmt, tm, len);
    if (str != NULL && len > 0)
    {
        insertPointer(pos, str, len);
    }
    return len;
}

//------------------------------------------------------------------------------
AQLogStringBuilder& AQLogStringBuilder::erase(const iterator& pos, size_t len)
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
    return *this;
}

//------------------------------------------------------------------------------
AQLogStringBuilder::iterator AQLogStringBuilder::find(const iterator& pos, char ch) const
{
    const iterator e = end();
    iterator it = pos;
    while (it != e && *it != ch)
    {
        it++;
    }
    return it;
}

//------------------------------------------------------------------------------
AQLogStringBuilder::iterator AQLogStringBuilder::find(const iterator& pos, const char *str) const
{
    // Get the length; refuse to match an empty string - move the iterator to
    // the end and return false.
    const iterator e = end();
    iterator it = pos;
    if (str != NULL && str[0] != '\0')
    {
        // Advance our itition - we will compare in reverse 
        size_t len = strlen(str);
        const iterator searchEnd = e - (len - 1);
        while (it < searchEnd)
        {
            if (*it == str[0])
            {
                const struct iovec &iov = m_vect[it.m_vectIdx];
                if (it.m_vectOff + len <= iov.iov_len)
                {
                    // Can do a fast compare with memcmp().
                    if (memcmp(((char *)iov.iov_base) + it.m_vectOff + 1, &str[1], len - 1) == 0)
                    {
                        // Found the item; return it.
                        return it;
                    }
                }
                else
                {
                    // Crosses boundaries - need to do it the old way.
                    iterator it2 = it + 1;
                    size_t i = 1;
                    while (i < len && *it2++ == str[i])
                    {
                        ++i;
                    }
                    if (i == len)
                    {
                        // Found the item - return it.
                        return it;
                    }
                }
            }
            ++it;
        }
    }

    // Reaching here indicates failure.
    return e;
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
void AQLogStringBuilder::split(const iterator& pos)
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

        // Splitting does not actually change where the iterator points in the
        // logical sense - however we need to update the iterator actual 
        // pointers due to the new split.
        const_cast<iterator&>(pos).m_vectIdx++;
        const_cast<iterator&>(pos).m_vectOff = 0;
        m_vect.insert(m_vect.begin() + pos.m_vectIdx, iov);
    }
}

//------------------------------------------------------------------------------
char *AQLogStringBuilder::allocString(size_t len)
{
    char *str;

    if (len <= SCOPY_SIZE - m_scopyPos)
    {
        // Allocate from the static buffer.
        str = &m_scopy[m_scopyPos];
        m_scopyPos += len;
    }
    else if (len <= m_dcopyFreeSize)
    {
        // Allocate from the most recent dynamic buffer.
        str = m_dcopyFree;
        m_dcopyFreeSize -= len;
        m_dcopyFree += len;
    }
    else
    {
        // Allocate a new dynamic buffer.
        if (m_dcopy == NULL)
        {
            m_dcopy = new vector<char *>();
        }

        uint32_t bitShift = m_dcopy->size();
        if (bitShift > DCOPY_MAX_SHIFT_BITS)
        {
            bitShift = DCOPY_MAX_SHIFT_BITS;
        }
        size_t minSize = DCOPY_FIRST_SIZE << bitShift;

        if (len >= minSize || minSize - len <= m_dcopyFreeSize)
        {
            // Don't overallocate - we get no benefit from it at this 
            // point.
            str = (char *)malloc(len);
        }
        else
        {
            // Overallocate and use the rest for the m_dcopyFree.
            str = (char *)malloc(minSize);
            m_dcopyFreeSize = minSize - len;
            m_dcopyFree = &str[len];
        }
        m_dcopy->push_back(str);
    }
    return str;
}

//------------------------------------------------------------------------------
const char *AQLogStringBuilder::allocStrftime(const char *fmt, const struct tm *tm,
    size_t& strLen)
{
    // Try to allocate into the static buffer first.
    size_t availLen = SCOPY_SIZE - m_scopyPos;
    if (availLen >= STRFTIME_SCOPY_MIN_SIZE)
    {
        strLen = strftime(&m_scopy[m_scopyPos], availLen, fmt, tm);
        if (strLen > 0)
        {
            const char *str = &m_scopy[m_scopyPos];
            m_scopyPos += strLen;
            return str;
        }
    }

    // Not enough space available.  Have to allocate dynamically.
    availLen = STRFTIME_DCOPY_BASE_SIZE;
    char *str = (char *)malloc(availLen);
    while ((strLen = strftime(str, availLen, fmt, tm)) == 0)
    {
        availLen <<= 1;
        char *newStr = (char *)realloc(str, availLen);
        if (newStr == NULL)
        {
            free(str);
            return NULL;
        }
        str = newStr;
    }
    char *newStr = (char *)realloc(str, strLen);
    if (newStr != NULL)
    {
        str = newStr;
    }

    // Add to the dynamic allocation list.
    if (m_dcopy == NULL)
    {
        m_dcopy = new vector<char *>();
    }
    m_dcopy->push_back(str);
    return str;
}

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const AQLogStringBuilder::iterator& it)
{
    os << "[" << it.m_vectIdx + ", +" << it.m_vectOff << "]";
    return os;
}

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const AQLogStringBuilder& sb)
{
    os << sb.toString();
    return os;
}




//=============================== End of File ==================================
