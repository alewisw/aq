#ifndef WORDWRAPPER_H
#define WORDWRAPPER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdint.h>




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class AQLogStringBuilder;




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Defines the default log formatter that is used when no log formatter is 
// specified for a given log handler.  The default log formatter is designed
// to produce human-readable text output.
//
// There are no configurable options for the default log formatter and for this
// reason it is not possible to directy construct run; rather simply do not set
// the log formatter for a particular handler and it will fall back to the
// default log formtter.
namespace aqlog { class WordWrapper
{
private:

    // Utility class - cannot be constructed, assigned or destroyed.
    WordWrapper(void);
    ~WordWrapper(void);

public:

    // Defines the types of newlines that can be used for word wrapping.
    enum Newline
    {
        // Use carriage return / linefeed pair (CR-LF, "\r\n") as found on Windows.
        NEWLINE_CRLF,

        // Use a linefeed (LF, "\n") as found on Unix.
        NEWLINE_LF,

        // Use the correct newline for the current platform.
        NEWLINE_PLATFORM,
    };

    // Appends the content of 'src' to 'dst' word wrapping as it is appended.
    // The following rules are applied:
    //  - Each line is prefixed with 'prefixSpaces' space characters.
    //  - The newline used is 'newline'.
    static void appendWordWrapped(const AQLogStringBuilder& src, 
        AQLogStringBuilder& dst, uint32_t wrapCols, uint32_t prefixSpaces, 
        Newline newline = NEWLINE_PLATFORM);

private:

    // Appends the prefix string to 'dst' where the prefix consists of 'prefixSpaces'
    // and starts with a newline offset 'prefixOffset' characters into the prefix
    // buffer.
    static void appendPrefix(AQLogStringBuilder& dst, uint32_t prefixSpaces, 
        uint32_t prefixOffset);

};}




#endif
//=============================== End of File ==================================
