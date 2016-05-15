//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "WordWrapper.h"

#include "AQLogStringBuilder.h"

#include <algorithm>

using namespace std;

namespace aqlog
{




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// The number of spaces in the Prefix string.
#define PREFIX_SPACES                   (sizeof(Prefix) - 3)




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

// Static prefix string buffer.
static const char Prefix[] = "\r\n"
        "                                                                      "
        "                                                                      "
        "                                                                      "
        "                                                                      ";



//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void WordWrapper::appendWordWrapped(const AQLogStringBuilder& src,
    AQLogStringBuilder& dst, uint32_t wrapCols, uint32_t prefixSpaces, 
    Newline newline)
{
    uint32_t basePrefixOffset;

    switch (newline)
    {
    default:
        newline = Newline::NEWLINE_LF;
        // falls through

    case Newline::NEWLINE_LF:
        basePrefixOffset = 1;
        break;

    case Newline::NEWLINE_CRLF:
        basePrefixOffset = 0;
        break;
    }


    AQLogStringBuilder::iterator lineStart = src.begin();
    AQLogStringBuilder::iterator it = lineStart;
    AQLogStringBuilder::iterator firstSpace = src.end();
    AQLogStringBuilder::iterator lastSpace = firstSpace;
    uint32_t cols = 0;
    while (it != src.end())
    {
        // Any of "\n", "\r\n", or "\n\r" result in a newline being inserted.
        char ch = *it;
        if (ch == '\r' || ch == '\n')
        {
            // Append the line.
            if (lastSpace == it)
            {
                dst.appendPointer(lineStart, firstSpace);
            }
            else
            {
                dst.appendPointer(lineStart, it);
            }

            // Check for a following '\r' or '\n' and skip over it for the
            // next line.
            it++;
            if (it != src.end())
            {
                char ch2 = *it;
                if (ch2 != ch && (ch2 == '\r' || ch2 == '\n'))
                {
                    it++;
                }
            }
            lineStart = it;
            firstSpace = lastSpace = src.end();
            cols = 0;

            // Attach the prefix for the next line unless we reached the end.
            if (it != src.end())
            {
                appendPrefix(dst, prefixSpaces, basePrefixOffset);
            }
        }
        else 
        {
            if (isspace(ch))
            {
                // Find the range of spaces until the end of the line or a non-space
                // character.
                firstSpace = it;
                do
                {
                    it++;
                    cols++;
                } while (it != src.end() && isspace(*it) && *it != '\r' && *it != '\n');
                lastSpace = it;
            }
            else
            {
                it++;
                cols++;
            }

            // Word wrap.
            if (cols >= wrapCols && firstSpace != src.end())
            {
                dst.appendPointer(lineStart, firstSpace);
                if (lastSpace != src.end())
                {
                    appendPrefix(dst, prefixSpaces, basePrefixOffset);
                }
                lineStart = lastSpace;
                firstSpace = lastSpace = src.end();
                cols = 0;
            }
        }
    }

    // Append the remainder of the string.
    if (lineStart < it)
    {
        if (lastSpace == it)
        {
            dst.appendPointer(lineStart, firstSpace);
        }
        else
        {
            dst.appendPointer(lineStart, it);
        }
    }
}

//------------------------------------------------------------------------------
void WordWrapper::appendPrefix(AQLogStringBuilder& dst, uint32_t prefixSpaces,
    uint32_t prefixOffset)
{
    // Okay, attach the prefix for the next line.
    prefixSpaces += 2 - prefixOffset;
    uint32_t n = 0;
    while (n < prefixSpaces)
    {
        size_t c = std::min(sizeof(Prefix) - prefixOffset - 1, prefixSpaces - n);
        dst.appendPointer(&Prefix[prefixOffset], c);
        n += c;
        prefixOffset = 2;
    }
}




}
//=============================== End of File ==================================
