#ifndef AQLOGSTRINGBUILDER_H
#define AQLOGSTRINGBUILDER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <string>
#include <vector>

#include <stdint.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * Foo
 */
class AQLogStringBuilder
{
private:

    // Contains a vectored component of this string.
    struct iovec
    {
        // The string.
        void *iov_base;

        // The length of the string.
        size_t iov_len;
    };

public:
    /**
    * Constructs a new string builder.  The string is empty.
    */
    AQLogStringBuilder(void);

    /**
    * Destroys this string builder.
    */
    ~AQLogStringBuilder(void);

    /**
    * Defines the iterator that is used to access and navigate a formatted
    * string.
    */
    class iterator : public std::iterator<std::bidirectional_iterator_tag, char>
    {
    private:

        // Constructs a new iterator with no associated data.
        iterator(void)
            : m_fm(NULL)
            , m_vectIdx(0)
            , m_vectOff(0)
        {
        }

    public:

        // Constructs a new iterator that iterates over the passed string builder.
        // The iterator points to the start of the string.
        iterator(AQLogStringBuilder& fm, size_t vectIdx = 0, size_t vectOff = 0)
            : m_fm(&fm)
            , m_vectIdx(vectIdx)
            , m_vectOff(vectOff)
        {
        }

        /**
        * Constructs a new iterator that is an exact copy of another iterator.
        *
        * @param other The other iterator.
        */
        iterator(const iterator& other)
            : m_fm(other.m_fm)
            , m_vectIdx(other.m_vectIdx)
            , m_vectOff(other.m_vectOff)
        {
        }

        /**
        * Assigns the value of this iterator so it exactly matches another iterator.
        *
        * @param other The other iterator.
        * @returns X
        */
        iterator &operator=(const iterator& other)
        {
            if (this != &other)
            {
                m_fm = other.m_fm;
                m_vectIdx = other.m_vectIdx;
                m_vectOff = other.m_vectOff;
            }
            return *this;
        }

        /**
        * Destroys this iterator.
        */
        ~iterator(void) { }

        /**
        * Determines if this iterator points to exactly the same position in the the
        * same string builder as another iterator.
        *
        * @param rhs The other iterator.
        * @return True if the two iterators are the same, false if they are different.
        * The return value is undefined if the two iterators point to different
        * strings.
        */
        bool operator==(const iterator& rhs) const
        {
            return rhs.m_vectIdx == m_vectIdx
                && rhs.m_vectOff == m_vectOff;
        }

        /**
        * Determines if this iterator is different to another iterator; that is it
        * points to a different string builder or to a different position in the
        * same string builder.
        *
        * @param rhs The other iterator.
        * @return True if the two iterators are different, true if they are the same.
        * The return value is undefined if the two iterators point to different
        * strings.
        */
        bool operator!=(const iterator& rhs) const
        {
            return !operator==(rhs);
        }

        /**
        * Determines if this iterator is at a point in the owning string that is
        * greater than another iterator.  If the two iterators point to different
        * string builders then the return value is undefined.
        *
        * @param rhs The other iterator.
        * @return True if the two iterators are for the same string builder and
        * this iterator is at a position further along than the other.
        */
        bool operator>(const iterator& rhs) const
        {
            return (m_vectIdx > rhs.m_vectIdx || (m_vectIdx == rhs.m_vectIdx && m_vectOff > rhs.m_vectOff));
        }

        /**
        * Determines if this iterator is at a point in the owning string that is
        * greater than or equal to another iterator.  If the two iterators point to different
        * string builders then the return value is undefined.
        *
        * @param rhs The other iterator.
        * @return True if the two iterators are for the same string builder and
        * this iterator is at a position the same as, or further along than the other.
        */
        bool operator>=(const iterator& rhs) const
        {
            return ((*this) == rhs || (*this) > rhs);
        }

        /**
        * Determines if this iterator is at a point in the owning string that is
        * less than another iterator.  If the two iterators point to different
        * string builders then the return value is undefined.
        *
        * @param rhs The other iterator.
        * @return True if the two iterators are for the same string builder and
        * this iterator is at a position that is not as far along as the other.
        */
        bool operator<(const iterator& rhs) const
        {
            return !((*this) >= rhs);
        }

        /**
        * Determines if this iterator is at a point in the owning string that is
        * less than or equal to another iterator.  If the two iterators point to different
        * string builders then the return value is undefined.
        *
        * @param rhs The other iterator.
        * @return True if the two iterators are for the same string builder and
        * this iterator is at a position that is the same as, or not as far along as
        * the other.
        */
        bool operator<=(const iterator& rhs) const
        {
            return ((*this) == rhs || (*this) < rhs);
        }

        /**
        * Obtains the character currently pointed to by this iterator.  This is not
        * valid if the iterator is pointing to the end of the string builder.
        *
        * @returns The character pointed to by this iterator.
        */
        char operator*(void) const
        {
            return ((char *)vect().iov_base)[m_vectOff];
        }

        /**
        * Moves this iterator forward by a single character in the string.
        *
        * @return This iterator.
        */
        iterator& operator++(void)
        {
            if (m_vectIdx < m_fm->m_vect.size())
            {
                if (m_vectOff < vect().iov_len - 1)
                {
                    // Simple case - another byte from the same array.
                    m_vectOff++;
                }
                else
                {
                    // Passed the end of the array, move to the next vector.
                    m_vectOff = 0;
                    m_vectIdx++;
                }
            }
            return *this;
        }

        /**
        * Moves this iterator forward by a single character in the string and
        * returns the previous value of the iterator.
        *
        * @param dummy Dummy integer, ignored.
        * @return This value of this iterator before it was moved.
        */
        iterator operator++(int dummy)
        {
            iterator it = *this;
            ++(*this);
            return it;
        }

        /**
        * Moves this iterator forward by a number of characters in the string.
        *
        * @param n The number of characters by which the iterator is to move
        * forward.
        * @return This iterator.
        */
        iterator& operator+=(size_t n)
        {
            while (n > 0 && m_vectIdx < m_fm->m_vect.size())
            {
                const struct iovec& iov = vect();
                size_t a = iov.iov_len - m_vectOff;
                if (n < a)
                {
                    // Terminal case - we terminate inside this vector.
                    m_vectOff += n;
                    n = 0;
                }
                else
                {
                    // Iterative case - move to the next vector.
                    n = n - (iov.iov_len - m_vectOff);
                    m_vectIdx++;
                    m_vectOff = 0;
                }
            }
            return *this;
        }

        /**
        * Creates a new iterator that is the same as this iterator except that it
        * is moved forward by a number of characters in the string.
        *
        * @param n The number of characters by which the new iterator is advanced.
        * @return The new iterator.
        */
        iterator operator+(size_t n) const
        {
            iterator it = *this;
            it += n;
            return it;
        }

        /**
        * Moves this iterator backwards by a single character in the string.
        *
        * @return This iterator.
        */
        iterator& operator--(void)
        {
            if (m_vectOff > 0)
            {
                // Simple case - another byte from the same array.
                m_vectOff--;
            }
            else if (m_vectIdx > 0)
            {
                // Not passed the end of the array and more vectors to come.
                m_vectIdx--;
                m_vectOff = vect().iov_len - 1;
            }
            return *this;
        }

        /**
        * Moves this iterator backwards by a single character in the string
        * and returns an iterator that points to the original location of this
        * iterator.
        *
        * @param dummy Dummy integer, ignored.
        * @return This value of this iterator before it was moved.
        */
        iterator operator--(int dummy)
        {
            iterator it = *this;
            --(*this);
            return it;
        }

        /**
        * Moves this iterator backwards by a number of characters in the string.
        *
        * @param n The number of characters by which the iterator is to move
        * backward.
        * @return This iterator.
        */
        iterator& operator-=(size_t n)
        {
            while (n > 0 && (m_vectIdx > 0 || m_vectOff > 0))
            {
                if (n <= m_vectOff)
                {
                    // Terminal case - we terminate inside this vector.
                    m_vectOff -= n;
                    n = 0;
                }
                else if (m_vectIdx == 0)
                {
                    // Terminal case - we reach the beginning.
                    m_vectOff = 0;
                    n = 0;
                }
                else
                {
                    // Iterative case - move to the next vector.
                    n -= m_vectOff + 1;
                    m_vectIdx--;
                    const struct iovec& iov = vect();
                    m_vectOff = iov.iov_len - 1;
                }
            }
            return *this;
        }

        /**
        * Creates a new iterator that is the same as this iterator except that it
        * is moved forward by a number of characters in the string.
        *
        * @param n The number of characters by which the new iterator is advanced.
        * @return The new iterator.
        */
        iterator operator-(size_t n) const
        {
            iterator it = *this;
            it -= n;
            return it;
        }

        /**
        * Finds the difference in position between two iterators.  The difference is
        * always positive regardless of the order of arguments passed (lhs / rhs).
        * If the two iterators point to different string builders then the return
        * value is undefined.
        *
        * @param rhs The other iterator to compare against to find the difference.
        * @returns The number of characters between the two iterator positions.
        */
        size_t operator-(const iterator& rhs) const
        {
            iterator lo;
            iterator hi;
            if ((*this) > rhs)
            {
                lo = rhs;
                hi = *this;
            }
            else
            {
                lo = *this;
                hi = rhs;
            }

            size_t total = 0;
            while (lo < hi)
            {
                if (lo.m_vectIdx == hi.m_vectIdx)
                {
                    // End of search - both in the same vector.
                    total += hi.m_vectOff - lo.m_vectOff;
                    lo = hi;
                }
                else
                {
                    // Advance lo further along.
                    const struct iovec& iov = lo.vect();
                    total += iov.iov_len - lo.m_vectOff;
                    lo.m_vectIdx++;
                    lo.m_vectOff = 0;
                }
            }

            return total;
        }

    private:

        // Returns the vector currently pointed at by this iterator.
        struct iovec& vect(void) const
        {
            return m_fm->m_vect[m_vectIdx];
        }

        // Allow string builder to directly manipulate these fields.
        friend class AQLogStringBuilder;

        // The string being iterated over.
        AQLogStringBuilder *m_fm;

        // The index of the current vector.
        size_t m_vectIdx;

        // Our offset within the string vector.
        size_t m_vectOff;

    };

    /**
    * Obtains the iterator that points to the start of this string.
    *
    * @return The iterator.
    */
    iterator begin(void) { return iterator(*this); }

    /**
    * Obtains the iterator that points to the end of this string.
    *
    * @return The iterator.
    */
    iterator end(void) { return iterator(*this, m_vect.size(), 0); }

    /**
    * Clears this string builder, reducing its size to 0.
    */
    void clear(void);

    /**
    * Inserts an empty string into this string at the specified position.
    * The string pointer is returned and can be filled in by the caller.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + len.  The iterator
    * itself is unchanged.
    * @param len The length of the string in bytes.  If this is zero then no
    * changes are made to the list and NULL is returned.
    * @returns The newly inserted buffer if len was greater than 0 or NULL if
    * len was 0.
    */
    char *insertEmpty(iterator& pos, size_t len);

    /**
    * Inserts a string into this string builder at the specified iterator position.
    * The string is copied and after this function returns it may
    * be discarded by the caller.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + len.  The iterator
    * itself is unchanged.
    * @param str The string to insert.  The string is copied and managed internally
    * from this point forwards.
    */
    void insertCopy(iterator& pos, const char *str)
    {
        insertCopy(pos, str, strlen(str));
    }

    /**
    * Inserts a string into this string builder at the specified iterator position.
    * The string is copied and after this function returns it may
    * be discarded by the caller.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + len.  The iterator
    * itself is unchanged.
    * @param str The string to insert.  The string is copied and managed internally
    * from this point forwards.
    * @param len The length of the string in bytes.
    */
    void insertCopy(iterator& pos, const char *str, size_t len);

    /**
    * Inserts a string into this string builder at the specified iterator position.
    * The string must have been allocated with 'malloc' (NOTE: not 'new').
    * Ownership of the string, and responsibility for calling 'free' is taken by
    * this class - the caller may not further access the string after this function
    * has been called.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + len.  The iterator
    * itself is unchanged.
    * @param str The string to insert.  It must have been allocated with 'malloc'
    * and ownership for the string is taken by this class.
    */
    void insertFree(iterator& pos, char *str)
    {
        insertFree(pos, str, strlen(str));
    }

    /**
    * Inserts a string into this string builder at the specified iterator position.
    * The string must have been allocated with 'malloc' (NOTE: not 'new').
    * Ownership of the string, and responsibility for calling 'free' is taken by
    * this class - the caller may not further access the string after this function
    * has been called.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + len.  The iterator
    * itself is unchanged.
    * @param str The string to insert.  It must have been allocated with 'malloc'
    * and ownership for the string is taken by this class.
    * @param len The length of the string in bytes.
    */
    void insertFree(iterator& pos, char *str, size_t len);

    /**
    * Inserts a constant string into this string builder at the specified
    * iterator position.  The string is not copied and must remain valid while
    * this string builder is in scope.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + len.  The iterator
    * itself is unchanged.
    * @param str The string to insert.  The string is not copied and must
    * remain valid and unchanged so long as this object is valid and in scope.
    */
    void insertPointer(iterator& pos, const char *str)
    {
        insertPointer(pos, str, strlen(str));
    }

    /**
    * Inserts a constant string into this string builder at the specified
    * iterator position.  The string is not copied and must remain valid while
    * this string builder is in scope.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + len.  The iterator
    * itself is unchanged.
    * @param str The string to insert.  The string is not copied and must
    * remain valid and unchanged so long as this object is valid and in scope.
    * @param len The length of the string in bytes.
    */
    void insertPointer(iterator& pos, const char *str, size_t len);

    /**
    * Erases part of this string.  The erasure occurs at a given byte offset
    * and is of a given length.
    *
    * @param pos The iterator where the erase occurs.  The character at this
    * position is erased.  The iterator itself is unchanged.
    * @param len The number of characters to erase.  If this goes beyond the
    * end of the string then everything after, and including, pos is erased.
    */
    void erase(iterator& pos, size_t len);

    /**
    * Advances the passed iterator until the specified character is found
    * or the end of the string is reached.
    *
    * @param pos The iterator to advance.  When this function returns this
    * either points to the position of a character with value 'ch' or it
    * points to end().
    * @param ch The character that is to be found.
    * @returns True if the character was found, or false if the character
    * was not found.  When true is returned pos points to the character,
    * when false is returned pos points to end().
    */
    bool find(iterator& pos, char ch);

    /**
    * Advances the passed iterator until the specified string is found
    * or the end of the string is reached.
    *
    * @param pos The iterator to advance.  When this function returns this
    * either points to the position of the first character in a string
    * equal to 'str' or it points to end().
    * @param str The string that is to be found.  If this argument is NULL
    * or the empty string then the search fails, false is returned, and
    * pos points to the end of the string builder.
    * @returns True if the string was found, or false if the string
    * was not found.  When true is returned pos points to the first
    * character of the string, when false is returned pos points to end().
    */
    bool find(iterator& pos, const char *str);

    /**
    * Obtains the total size of the string builder in characters.  No
    * nul terminator is included in this size.
    *
    * @returns The total size of the string builder.
    */
    size_t size(void) const { return m_totalSize; }

    /**
    * Obtains the number of individual I/O vectors that make up this string builder.
    *
    * @returns The number of I/O vectors.
    */
    size_t iovCount(void) const { return m_vect.size(); }

    /**
    * Writes this entire string builder into a character array.  The character array
    * is provided by the caller of a fixed size.  If the entire string builder cannot
    * fit in the character array then only that part which can fit is written.
    * If the length of the supplied character array is larger than the
    * required space the character array is nul terminated as a C-style string
    * otherwise no nul termination character is written.
    *
    * @param dst The character array into which the data is to be written.
    * @param dstLen The length of the character array.
    */
    void toCharArray(char *dst, size_t dstLen) const;

    /**
    * Obtains a copy of this entire string builder as a string.
    *
    * @return The string that contains this entire string builder.
    */
    std::string toString(void) const;

private:

    // Splits the string vector at 'pos' such that the content of the
    // string builder is unchanged, but the iterator pos now reference the
    // start of a vector.
    void split(iterator& pos);

    // Allocates a new string of length 'len' bytes for insertion into the 
    // buffer.
    char *allocString(size_t len);



    // The number of bytes to allocated statically for the static copy buffer.
    static const int SCOPY_SIZE = 64;

    // The array of vectors holding buffers in this string builder.
    std::vector<struct iovec> m_vect;

    // The statically allocated buffer for holding strings that were copied into this string builder.
    char m_scopy[SCOPY_SIZE];

    // The position of the next copy out of the static copy buffer.
    size_t m_scopyPos;

    // The array of dynamically allocated buffers holding strings that were copied into this string builder.
    std::vector<char *> *m_dcopy;

    // The total size of the string builder.
    size_t m_totalSize;

};




#endif
//=============================== End of File ==================================
