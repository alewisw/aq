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

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <ostream>
#include <vector>
#include <string>




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
* Provides functions for constructing a string as series of linked buffers,
* avoiding buffer copying on string expansion or insertion.
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
        iterator(const AQLogStringBuilder& fm, size_t vectIdx = 0, size_t vectOff = 0)
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
        * @returns A reference to this iterator.
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
        const struct iovec& vect(void) const
        {
            return m_fm->m_vect[m_vectIdx];
        }

        // Allow string builder to directly manipulate these fields.
        friend class AQLogStringBuilder;

        // Allow the output stream to access the iterator state.
        friend std::ostream& operator<<(std::ostream& os, const AQLogStringBuilder::iterator& it);

        // The string being iterated over.
        const AQLogStringBuilder *m_fm;

        // The index of the current vector.
        size_t m_vectIdx;

        // Our offset within the string vector.
        size_t m_vectOff;

    };

    // If this option is set then adjacent iov structures are automatically coalesced
    // into single structures.  It is enabled by default and should only be turned off
    // for the unit test runs.
    static const uint32_t OPTION_COALESCE_ADJACENT_IOV = (1 << 0);

    /**
    * Constructs a new string builder.  The string builder is empty.
    */
    AQLogStringBuilder(void);

    // Constructs a new string builder with a configurable set of options.  This
    // only exists for use in the unit tests.
    AQLogStringBuilder(uint32_t options);

    /**
    * Constructs a new string builder that contains exactly the content of
    * another string builder.  The content of the other string builder is entirely
    * duplicated as if appendCopy(const AQLogStringBuilder&) had been called on it,
    * passed `other` as the only parameter.  This can result in undue copying
    * of memory if it is guaranteed that the other string builder will not go
    * out of scope before this string builder goes out of scope.  If this is the
    * case it is recommended that a new string builder is constructed and then
    * appendPointer(const AQLogStringBuilder&) called on that new instance.
    *
    * @param other The other string builder to completly copy.
    */
    AQLogStringBuilder(const AQLogStringBuilder& other);

    /**
    * Changes this string builder so that it contains exactly the same data as
    * another string builder.  This results in this builder first being cleared
    * then all the memory from the other builder being copied into this builder.
    *
    * This can result in undue copying of memory if it is guaranteed that the
    * other string builder will not go out of scope before this string builder
    * goes out of scope.  If this is the case it is recommended that the
    * functions clear() followed by appendPointer(const AQLogStringBuilder&) are
    * called on this string builder instead.
    *
    * @param other The other string builder.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& operator=(const AQLogStringBuilder& other);

    /**
    * Destroys this string builder.
    */
    ~AQLogStringBuilder(void);

    /**
    * Obtains the iterator that points to the start of this string.
    *
    * @return The iterator.
    */
    iterator begin(void) const { return iterator(*this); }

    /**
    * Obtains the iterator that points to the end of this string.
    *
    * @return The iterator.
    */
    iterator end(void) const { return iterator(*this, m_vect.size(), 0); }

    /**
    * Clears this string builder, reducing its size to 0.
    *
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& clear(void);

    /**
    * Appends an empty string to this string builder.  The string pointer
    * is returned  and can be filled in by the caller.
    *
    * @param len The length of the string in bytes.  If this is zero then no
    * changes are made to the list and NULL is returned.
    * @returns The newly appended buffer if len was greater than 0 or NULL if
    * len was 0.
    */
    char *appendEmpty(size_t len);

    /**
    * Appends a string to the end of this string builder. The string is copied
    * and after this function returns it may be discarded by the caller.
    *
    * @param str The string to append.  The string is copied and managed internally
    * from this point forwards.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendCopy(const char *str)
    {
        if (str != NULL)
        {
            appendCopy(str, strlen(str));
        }
        return *this;
    }

    /**
    * Appends a string to the end of this string builder. The string is copied
    * and after this function returns it may be discarded by the caller.
    *
    * @param str The string to append.  The string is copied and managed internally
    * from this point forwards.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendCopy(const std::string &str)
    {
        appendCopy(str.c_str(), str.size());
        return *this;
    }

    /**
    * Appends a string to the end of this string builder. The string is copied
    * and after this function returns it may be discarded by the caller.
    *
    * @param str The string to append.  The string is copied and managed internally
    * from this point forwards.
    * @param len The length of the string in bytes.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendCopy(const char *str, size_t len);

    /**
    * Appends the entire content from another string builder to this string builder.
    * The character data is copied this string builder so after this function returns
    * the other string builder may be discarded.
    *
    * @param sb The string builder to copy.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendCopy(const AQLogStringBuilder& sb)
    {
        appendCopy(sb.begin(), sb.end());
        return *this;
    }

    /**
    * Appends a range of characters from another string builder to this string
    * builder.  The characters are copied into this string builder so after this
    * function returns the other string builder may be discarded.
    *
    * @param sbBegin An iterator referencing the first character in the other string
    * build from which the characters are to be appended.  This may reference this
    * string builder in order to duplicate a character range.
    * @param sbEnd An iterator reference one position past the last character in the
    * other string builder that is to be appended.  This must reference the same
    * string builder as sbBegin or the behavior is undefined.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendCopy(const iterator& sbBegin, const iterator& sbEnd)
    {
        insertCopy(end(), sbBegin, sbEnd);
        return *this;
    }

    /**
    * Appends a string to the end of this string builder.
    * The string must have been allocated with 'malloc' (NOTE: not 'new').
    * Ownership of the string, and responsibility for calling 'free' is taken by
    * this class - the caller may not further access the string after this function
    * has been called.
    *
    * @param str The string to append.  It must have been allocated with 'malloc'
    * and ownership for the string is taken by this class.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendFree(char *str)
    {
        if (str != NULL)
        {
            appendFree(str, strlen(str));
        }
        return *this;
    }

    /**
    * Appends a string to the end of this string builder.
    * The string must have been allocated with 'malloc' (NOTE: not 'new').
    * Ownership of the string, and responsibility for calling 'free' is taken by
    * this class - the caller may not further access the string after this function
    * has been called.
    *
    * @param str The string to append.  It must have been allocated with 'malloc'
    * and ownership for the string is taken by this class.
    * @param len The length of the string in bytes.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendFree(char *str, size_t len);

    /**
    * Appends a constant string to the end of this string builder.  The string is
    * not copied and must remain valid while this string builder is in scope.
    *
    * @param str The string to append.  The string is not copied and must
    * remain valid and unchanged so long as this object is valid and in scope.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendPointer(const char *str)
    {
        if (str != NULL)
        {
            appendPointer(str, strlen(str));
        }
        return *this;
    }

    /**
    * Appends a constant string to the end of this string builder.  The string is
    * not copied and must remain valid while this string builder is in scope.
    *
    * @param str The string to append.  The string is not copied and must
    * remain valid and unchanged so long as this object is valid and in scope.
    * @param len The length of the string in bytes.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendPointer(const char *str, size_t len);

    /**
    * Appends an entire string builder to this string builder.  The characters are
    * not copied and as such the other string builder must remain valid while this
    * string builder is in scope.
    *
    * @param sb The string builder to append to this string builder.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendPointer(const AQLogStringBuilder& sb)
    {
        appendPointer(sb.begin(), sb.end());
        return *this;
    }

    /**
    * Appends a range of characters from another string builder into this string
    * builder.  The characters are not copied and as such the other string
    * builder must remain valid while this string builder is in scope.
    *
    * @param sbBegin An iterator referencing the first character in the other string
    * build from which the characters are to be appended.  This may reference this
    * string builder in order to duplicate a character range.
    * @param sbEnd An iterator reference one position past the last character in the
    * other string builder that is to be appended.  This must reference the same
    * string builder as sbBegin or the behavior is undefined.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendPointer(const iterator& sbBegin, const iterator& sbEnd)
    {
        insertPointer(end(), sbBegin, sbEnd);
        return *this;
    }


    /**
    * Appends a formatted string to this string builder.  The string is
    * formatted using the standard printf-style formatting string and
    * formatting arguments
    *
    * @param fmt The string format.
    * @param ... The formatting arguments.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendf(const char *fmt, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 2, 3)))
#endif
    {
        va_list argp;
        va_start(argp, fmt);
        vappendf(fmt, argp);
        va_end(argp);
        return *this;
    }

    /**
    * Appends a formatted string to this string builder.  The string is
    * formatted using the standard printf-style formatting string and
    * formatting arguments
    *
    * @param fmt The string format.
    * @param argp The formatting arguments.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& vappendf(const char *fmt, va_list argp);

    /**
    * Appends a formatted time string to this string builder.  The time
    * formatter uses the same rules as the strftime() function.
    *
    * @param fmt The string format.
    * @param time The time to format.  This will be converted into a
    * local time using the localtime() or similar function.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendftime(const char *fmt, time_t time);

    /**
    * Appends a formatted time string to this string builder.  The time
    * formatter uses the same rules as the strftime() function.
    *
    * @param fmt The string format.
    * @param tm The broken-down time to format according to the formatting
    * string.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& appendftime(const char *fmt, const struct tm *tm);

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
    char *insertEmpty(const iterator& pos, size_t len);

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
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertCopy(const iterator& pos, const char *str)
    {
        if (str != NULL)
        {
            insertCopy(pos, str, strlen(str));
        }
        return *this;
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
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertCopy(const iterator& pos, const std::string& str)
    {
        insertCopy(pos, str.c_str(), str.size());
        return *this;
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
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertCopy(const iterator& pos, const char *str, size_t len);

    /**
    * Inserts an entire string builder into this string builder.  The characters are
    * copied into this string builder so after this function returns the other string
    * builder may be discarded.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + (sbEnd - sbBegin + 1).
    * The iterator itself is unchanged.
    * @param sb The other string builder to insert into this string builder.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertCopy(const iterator& pos, const AQLogStringBuilder& sb)
    {
        insertCopy(pos, sb.begin(), sb.end());
        return *this;
    }

    /**
    * Inserts a range of characters from another string builder into this string
    * builder.  The characters are copied into this string builder so after this
    * function returns the other string builder may be discarded.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + (sbEnd - sbBegin + 1).
    * The iterator itself is unchanged.
    * @param sbBegin An iterator referencing the first character in the other string
    * build from which the characters are to be inserted.  This may reference this
    * string builder in order to duplicate a character range.
    * @param sbEnd An iterator reference one position past the last character in the
    * other string builder that is to be inserted.  This must reference the same
    * string builder as sbBegin or the behavior is undefined.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertCopy(const iterator& pos, const iterator& sbBegin,
        const iterator& sbEnd);

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
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertFree(const iterator& pos, char *str)
    {
        if (str != NULL)
        {
            insertFree(pos, str, strlen(str));
        }
        return *this;
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
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertFree(const iterator& pos, char *str, size_t len);

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
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertPointer(const iterator& pos, const char *str)
    {
        if (str != NULL)
        {
            insertPointer(pos, str, strlen(str));
        }
        return *this;
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
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertPointer(const iterator& pos, const char *str, size_t len);

    /**
    * Inserts an entire string builer into this string builder.  The characters
    * are not copied and as such the other string builder must remain valid while
    * this string builder is in scope.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + (sbEnd - sbBegin + 1).
    * The iterator itself is unchanged.
    * @param sb The string builder to insert into this string builder.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertPointer(const iterator& pos, const AQLogStringBuilder& sb)
    {
        insertPointer(pos, sb.begin(), sb.end());
        return *this;
    }

    /**
    * Inserts a range of characters from another string builder into this string
    * builder.  The characters are not copied and as such the other string
    * builder must remain valid while this string builder is in scope.
    *
    * @param pos The iterator where the string is to be inserted.  The character
    * currently at the iterator position is now moved to pos + (sbEnd - sbBegin + 1).
    * The iterator itself is unchanged.
    * @param sbBegin An iterator referencing the first character in the other string
    * build from which the characters are to be inserted.  This may reference this
    * string builder in order to duplicate a character range.
    * @param sbEnd An iterator reference one position past the last character in the
    * other string builder that is to be inserted.  This must reference the same
    * string builder as sbBegin or the behavior is undefined.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& insertPointer(const iterator& pos, const iterator& sbBegin,
        const iterator& sbEnd);

    /**
    * Inserts a formatted string to this string builder at a specified
    * location.  The string is formatted using the standard printf-style
    * formatting string and formatting arguments
    *
    * @param pos The position where the formatted string is to be written.
    * The first byte of the formatted string is placed at this location.
    * @param fmt The string format.
    * @param ... The formatting arguments.
    * @returns The total number of bytes inserted into the string.
    */
    size_t insertf(const iterator& pos, const char *fmt, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 4)))
#endif
    {
        va_list argp;
        va_start(argp, fmt);
        size_t len = vinsertf(pos, fmt, argp);
        va_end(argp);
        return len;
    }

    /**
    * Inserts a formatted string to this string builder at a specified
    * location.  The string is formatted using the standard printf-style
    * formatting string and formatting arguments
    *
    * @param pos The position where the formatted string is to be written.
    * The first byte of the formatted string is placed at this location.
    * @param fmt The string format.
    * @param argp The formatting arguments.
    * @returns The total number of bytes inserted into the string.
    */
    size_t vinsertf(const iterator& pos, const char *fmt, va_list argp);

    /**
    * Inserts a formatted time string to this string builder.  The time
    * formatter uses the same rules as the strftime() function and is
    * inserted at the specified location.
    *
    * @param pos The position where the formatted time is to be written.
    * The first byte of the formatted time is placed at this location.
    * @param fmt The string format.
    * @param time The time to format.  This will be converted into a
    * local time using the localtime() or similar function.
    * @returns A reference to this string builder.
    */
    size_t insertftime(const iterator& pos, const char *fmt, time_t time);

    /**
    * Inserts a formatted time string to this string builder.  The time
    * formatter uses the same rules as the strftime() function and is
    * inserted at the specified location.
    *
    * @param pos The position where the formatted time is to be written.
    * The first byte of the formatted time is placed at this location.
    * @param fmt The string format.
    * @param tm The broken-down time to format according to the formatting
    * string.
    * @returns A reference to this string builder.
    */
    size_t insertftime(const iterator& pos, const char *fmt, const struct tm *tm);

    /**
    * Erases part of this string.  The erasure occurs at a given byte offset
    * and is of a given length.
    *
    * @param pos The iterator where the erase occurs.  The character at this
    * position is erased.  The iterator itself is unchanged.
    * @param len The number of characters to erase.  If this goes beyond the
    * end of the string then everything after, and including, pos is erased.
    * @returns A reference to this string builder.
    */
    AQLogStringBuilder& erase(const iterator& pos, size_t len);

    /**
    * Searches the string builder for a specified character, returning an
    * iterator that references the first instance of that character.
    *
    * @param ch The character that is to be found.
    * @returns An iterator that references the character or end() if the
    * character was not found.
    */
    iterator find(char ch)
    {
        return find(begin(), ch);
    }

    /**
    * Searches the string builder for a specified character after a specified
    * position, returning an iterator that references the first instance of
    * that character starting from the given position.
    *
    * @param pos The position where the search is to begin.
    * @param ch The character that is to be found.
    * @returns An iterator that references the character or end() if the
    * character was not found.
    */
    iterator find(const iterator& pos, char ch);

    /**
    * Searches the string builder for a specified string, returning an
    * iterator that references the first character in the string.
    *
    * @param str The string that is to be found.  If this argument is NULL
    * or the empty string then the search fails, false is returned, and
    * pos points to the end of the string builder.
    * @returns An iterator that references the first character in the
    * string or end() if the string was not found.
    */
    iterator find(const std::string &str)
    {
        return find(str.c_str());
    }

    /**
    * Searches the string builder for a specified string, returning an
    * iterator that references the first character in the string.
    *
    * @param str The string that is to be found.  If this argument is NULL
    * or the empty string then the search fails, false is returned, and
    * pos points to the end of the string builder.
    * @returns An iterator that references the first character in the
    * string or end() if the string was not found.
    */
    iterator find(const char *str)
    {
        return find(begin(), str);
    }

    /**
    * Searches the string builder for a specified string, returning an
    * iterator that references the first character in the string.
    *
    * @param pos The position where the search is to begin.
    * @param str The string that is to be found.  If this argument is NULL
    * or the empty string then the search fails, false is returned, and
    * pos points to the end of the string builder.
    * @returns An iterator that references the first character in the
    * string or end() if the string was not found.
    */
    iterator find(const iterator& pos, const std::string& str)
    {
        return find(pos, str.c_str());
    }

    /**
    * Searches the string builder for a specified string, returning an
    * iterator that references the first character in the string.
    *
    * @param pos The position where the search is to begin.
    * @param str The string that is to be found.  If this argument is NULL
    * or the empty string then the search fails, false is returned, and
    * pos points to the end of the string builder.
    * @returns An iterator that references the first character in the
    * string or end() if the string was not found.
    */
    iterator find(const iterator& pos, const char *str);

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
    void split(const iterator& pos);

    // Allocates a new string of length 'len' bytes for insertion into the 
    // buffer.
    char *allocString(size_t len);

    // Allocates a new string to hold the formatted string time of 'tm' via
    // the 'strftime' function.  The new string is returned and its length
    // is placed in 'strLen'.
    const char *allocStrftime(const char *fmt, const struct tm *tm,
        size_t& strLen);

    // The number of bytes to allocated statically for the static copy buffer.
    static const size_t SCOPY_SIZE = 64;

    // The number of bytes to allocate to the first dynamic copy buffer.
    static const size_t DCOPY_FIRST_SIZE = 256;

    // When allocating dcopy buffers after the first, each buffer is 
    // DCOPY_FIRST_SIZE << (number of buffers) with a maximum of this value.
    static const uint32_t DCOPY_MAX_SHIFT_BITS = 8;

    // The configuration options for this string builder.
    uint32_t m_options;

    // The array of vectors holding buffers in this string builder.
    std::vector<struct iovec> m_vect;

    // The statically allocated buffer for holding strings that were copied into this string builder.
    char m_scopy[SCOPY_SIZE];

    // The position of the next copy out of the static copy buffer.
    size_t m_scopyPos;

    // The array of dynamically allocated buffers holding strings that were copied into this string builder.
    std::vector<char *> *m_dcopy;

    // Pointer to the first free (unused) byte in a dynamically allocated
    // character array.  The array (start) is already in the m_dcopy
    // list.
    char *m_dcopyFree;

    // The amount of space remaining in the m_dcopyFree dynamically allocated character
    // array.
    size_t m_dcopyFreeSize;

    // The total size of the string builder.
    size_t m_totalSize;

};

// Provides the output stream operator for a string builder iterator.
extern std::ostream& operator<<(std::ostream& os, const AQLogStringBuilder::iterator& it);

// Provides the output stream operator for a string builder.
extern std::ostream& operator<<(std::ostream& os, const AQLogStringBuilder& sb);



#endif
//=============================== End of File ==================================
