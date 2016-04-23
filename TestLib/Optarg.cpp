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

#include <iomanip>

using namespace std;




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// The width of the various columns in the help message.
#define HELP_OPT_WIDTH                  2
#define HELP_VALUE_WIDTH                11
#define HELP_DESC_WIDTH                 (80 - HELP_OPT_WIDTH - HELP_VALUE_WIDTH - 3)




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
void Optarg::updateHelp(char ch, const char *desc, const char *type, 
    const string def)
{
    // When no description is provided do not update the help message.
    if (desc == NULL)
    {
        return;
    }

    m_help << "-" << ch << " " 
        << left << setw(HELP_VALUE_WIDTH) << (type != NULL ? type : "") << " ";

    string str(desc);

    // Remove trailing spaces.
    while (str.size() > 0 && isspace(str[str.size() - 1]))
    {
        str.pop_back();
    }

    // Add default value description.
    if (type != NULL && def.size() > 0)
    {
        str += "  The default value is ";
        str += def;
        str += ".";
    }

    if (str.size() == 0)
    {
        m_help << endl;
    }
    else
    {
        // Word wrap to HELP_DESC_WIDTH.
        size_t pos = 0;
        while (str.size() - pos > 0)
        {
            if (str.size() - pos <= HELP_DESC_WIDTH)
            {
                m_help << str.substr(pos) << endl;
                pos = str.size();
            }
            else
            {
                // Find the first non-space character before a space.
                size_t end = pos + HELP_DESC_WIDTH;
                bool foundSpace = false;
                while (end > pos && !(foundSpace && !isspace(str[end])))
                {
                    if (isspace(str[end]))
                    {
                        foundSpace = true;
                    }
                    --end;
                }

                // If no space found then just put as much as we can.
                if (end == pos)
                {
                    end = pos + HELP_DESC_WIDTH;
                }
                m_help << str.substr(pos, end - pos + 1) << endl;
                pos = end + 1;

                // While we see white-space, skip it.
                while (pos < str.size() && isspace(str[pos]))
                {
                    pos++;
                }

                // Is there another line?  If so insert the prefix now.
                if (pos < str.size())
                {
                    m_help << setw(HELP_VALUE_WIDTH + HELP_OPT_WIDTH + 2) << "";
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
bool Optarg::hasOpt(char ch, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    updateHelp(ch, desc, NULL);

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
    updateHelp(ch, desc, "<string>", value);
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, bool& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend())
    {
        value = !value;
    }

    updateHelp(ch, desc, NULL);
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, int& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = (int)strtol(it->second.c_str(), NULL, 10);
    }

    ostringstream ss;
    ss << value;
    updateHelp(ch, desc, "<int>", ss.str());
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, unsigned int& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = (unsigned int)strtoul(it->second.c_str(), NULL, 10);
    }

    ostringstream ss;
    ss << value;
    updateHelp(ch, desc, "<uint>", ss.str());
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, long& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = (long)strtol(it->second.c_str(), NULL, 10);
    }

    ostringstream ss;
    ss << value;
    updateHelp(ch, desc, "<long>", ss.str());
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, unsigned long& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value = (unsigned long)strtoul(it->second.c_str(), NULL, 10);
    }

    ostringstream ss;
    ss << value;
    updateHelp(ch, desc, "<ulong>", ss.str());
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
    updateHelp(ch, desc, NULL);
}

//------------------------------------------------------------------------------
void Optarg::opt(char ch, std::vector<unsigned int>& value, const char *desc)
{
    map<char, string>::const_iterator it = m_opts.find(ch);

    ostringstream ss;
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (i > 0)
        {
            ss << ",";
        }
        ss << value[i];
    }

    if (it != m_opts.cend() && it->second.size() != 0)
    {
        value.clear();

        string s = it->second;

        char *rem = (char *)s.c_str();
        rem--;
        do
        {
            rem++;
            char *tmp;
            unsigned int v = (unsigned int)strtoul(rem, &tmp, 10);
            if (tmp == rem)
            {
                break;
            }
            else
            {
                value.push_back(v);
                rem = tmp;
            }
        } while (rem == NULL || *rem == ',' || *rem == ':' || *rem == ';');
    }

    updateHelp(ch, desc, "<uint-list>", ss.str());
}




//=============================== End of File ==================================
