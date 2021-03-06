#ifndef AQUNFORMATTEDEXCEPTION_H
#define AQUNFORMATTEDEXCEPTION_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdexcept>




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
 * The exception that is thrown when an attempt is made to access an unformatted
 * queue.
 */
class AQUnformattedException : public std::runtime_error
{
public:

    /**
     * Constructs a new exception with the given message.
     *
     * @param msg The message; this will be available through 
     * the std::exception::what() function.
     */
    AQUnformattedException(const std::string& msg);

    /**
     * Constructs this exception as an exact copy of another exception.
     * 
     * @param other The other exception to copy.
     */
    AQUnformattedException(const AQUnformattedException& other);

    /**
     * Assigns the value of this exception to exactly match another.
     *
     * @param other The other exception to copy.
     * @return A reference to this object.
     */
    AQUnformattedException& operator=(const AQUnformattedException& other);

    /**
     * Destroys this exception.
     */
    virtual ~AQUnformattedException(void) throw ();

};




#endif
//=============================== End of File ==================================
