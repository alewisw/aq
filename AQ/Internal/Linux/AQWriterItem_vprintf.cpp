//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQWriterItem.h"

#include "AQWriter.h"

#include <stdexcept>

#include <stdio.h>

using namespace aq;
using namespace std;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------

// The argument passed to the cookie file stream functions.
typedef struct COOKIE_T
{
    // The item being written.
    AQWriterItem *item;
    
    // The writing offset.
    size_t off;
    
    // The result of the operation, negative if the print was truncated.
    int result;
    
} Cookie_t;




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Cookie printf function for reading; since no reading should ever occur this
// always returns -1.
static ssize_t CookieReader(void *cookie, char *buffer, size_t size);

// Cookie printf function for writing.
static ssize_t CookieWriter(void *cookie, const char *buffer, size_t size);

// Cookie printf function for seeking; since seeking is not supported this always
// returns -1.
static int CookieSeeker(void *cookie, off64_t *position, int whence);

// Cookie printf function for cleaning; this always returns 0 and takes no action.
static int CookieCleaner(void *cookie);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

// Used to perform the printf operation into the AQWriterItem.
static const cookie_io_functions_t PrintfCookie =
{
    CookieReader,
    CookieWriter,
    CookieSeeker,
    CookieCleaner
};




//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int AQWriterItem::vprintf(size_t off, const char *fmt, va_list argp)
{
    // Validate arguments.
    if (!isAllocated())
    {
        throw domain_error("Cannot write to an AQWriterItem that has not been claimed() or has already been commit()'ed.");
    }
    if (fmt == NULL)
    {
        throw invalid_argument("Cannot printf with fmt set to NULL");
    }
    
    Cookie_t cookie;
    cookie.item = this;
    cookie.off = off;
    cookie.result = 0;
    
    FILE *f = fopencookie(&cookie, "w", PrintfCookie);
    if (f == NULL)
    {
        // Open has failed; just indicate that we wrote 0 bytes.
        return ~0;
    }
    
    vfprintf(f, fmt, argp);
    
    fclose(f);
    
    return cookie.result;
}

//------------------------------------------------------------------------------
static ssize_t CookieReader(void *cookie, char *buffer, size_t size)
{
    return -1;
}

//------------------------------------------------------------------------------
static ssize_t CookieWriter(void *cookie, const char *buffer, size_t size)
{
    Cookie_t *c = (Cookie_t *)cookie;
    
    if (c->result >= 0)
    {
        size_t written = c->item->write(c->off, buffer, size, AQWriterItem::WRITE_PARTIAL);
        
        c->off += written;
        c->result += written;
        if (written != size)
        {
            c->result = ~c->result;
        }
    }
    return size;
}

//------------------------------------------------------------------------------
static int CookieSeeker(void *cookie, off64_t *position, int whence)
{
    return -1;
}

//------------------------------------------------------------------------------
static int CookieCleaner(void *cookie)
{
    return 0;
}




//=============================== End of File ==================================
