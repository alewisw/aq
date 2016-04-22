#ifndef OPTARG_H
#define OPTARG_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <cstdint>

#include <map>
#include <string>
#include <sstream>
#include <vector>



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

// Command line options & arguments parser.  Parses all options and makes them 
// available for evaluation.
class Optarg
{
public:

    // Constructs a new options & argument set.   This tries to gather the set
    // of options and arguments that are all of the form:
    //   -c<value>
    // or
    //   -c <value>
    //
    // Multiple arguments cannot be combined.
    Optarg(int argc, char* argv[]);

    // Creates an exact copy of this options & argument set.
    Optarg(const Optarg& other);

    // Assigns the value of this options & argument set to exactly match another.
    Optarg& operator=(const Optarg& other);

    // Destroys this record generator.
    ~Optarg(void);

private:

    // The options and their arguments.
    std::map<char, std::string> m_opts;

    // The help message.
    std::ostringstream m_help;

    // Updates the help message for an option 'ch' with description 'desc'
    // of type 'type'.  If 'hasValue' is true then the option has a value.
    void updateHelp(char ch, const char *desc, const char *type, bool hasValue = true);

public:

    // Gets the help message for this options arguments.
    std::string helpMessage(void) const { return m_help.str(); }

    // Returns true if the passed option exists.
    bool hasOpt(char ch) const;

    // Updates the field 'value' with the option configured for 'ch' if
    // it exists.  If a description is provided it is used to update the
    // internal help message.
    void opt(char ch, std::string& value, const char *desc = NULL);
    void opt(char ch, bool& value, const char *desc = NULL);
    void opt(char ch, int& value, const char *desc = NULL);
    void opt(char ch, unsigned int& value, const char *desc = NULL);
    void opt(char ch, long& value, const char *desc = NULL);
    void opt(char ch, unsigned long& value, const char *desc = NULL);

    // Used to set a bit in a configuration bit-mask.
    void opt(char ch, unsigned int bitmask, unsigned int& value, const char *desc = NULL);

    // Used to set a comma-separated list.
    void opt(char ch, std::vector<unsigned int>& value, const char *desc = NULL);



};



#endif
//=============================== End of File ==================================
