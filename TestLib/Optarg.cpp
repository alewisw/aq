//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Optarg.h"

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
Optarg::Optarg(int argc, char* argv[])
{
    int i = 1;
    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            char ch = argv[i][1];
            if (ch > 32 && ch <= 127)
            {
                if (argv[i][2] != '\0')
                {
                    m_opts[ch] = &argv[i][2];
                }
                else if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    i++;
                    m_opts[ch] = argv[i];
                }
                else
                {
                    m_opts[ch] = "";
                }
            }
        }
        i++;
    }
}

//------------------------------------------------------------------------------
Optarg::Optarg(const Optarg& other)
    : m_opts(other.m_opts)
{
}

//------------------------------------------------------------------------------
Optarg& Optarg::operator=(const Optarg& other)
{
    if (this != &other)
    {
        m_opts = other.m_opts;
    }
    return *this;
}

//------------------------------------------------------------------------------
Optarg::~Optarg(void)
{
}

//------------------------------------------------------------------------------
void Optarg::updateHelp(char ch, const char *desc, const char *type, bool hasValue)
{
    // TODO
}

//------------------------------------------------------------------------------
bool Optarg::hasOpt(char ch) const
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    return it != m_opts.cend();
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, std::string& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = it->second;
    }
    updateHelp(ch, desc, "string");
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, bool& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend())
    {
        value = !value;
    }

    updateHelp(ch, desc, "bool", false);
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, int& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = (int)strtol(it->second.c_str(), NULL, 10);
    }
    updateHelp(ch, desc, "int");
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, unsigned int& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = (unsigned int)strtoul(it->second.c_str(), NULL, 10);
    }
    updateHelp(ch, desc, "uint");
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, long& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = (long)strtol(it->second.c_str(), NULL, 10);
    }
    updateHelp(ch, desc, "long");
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, unsigned long& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = (unsigned long)strtoul(it->second.c_str(), NULL, 10);
    }
    updateHelp(ch, desc, "ulong");
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, unsigned int bitmask, unsigned int& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend())
    {
        if (value & bitmask)
        {
            value &= ~bitmask;
        }
        else
        {
            value |= bitmask;
        }
    }
    updateHelp(ch, desc, "bool", false);
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, std::vector<unsigned int>& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value.clear();

        char *rem = (char *)(it->second.c_str()[-1]);
        do
        {
            rem++;
            value.push_back((unsigned int)strtoul(rem, &rem, 10));
        } while (*rem == ',');
    }

    updateHelp(ch, desc, "uint-list");
}



//=============================== End of File ==================================
