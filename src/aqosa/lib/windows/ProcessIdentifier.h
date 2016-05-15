#ifndef PROCESSIDENTIFIER_H
#define PROCESSIDENTIFIER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <Windows.h>

#include <stdint.h>

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

// Used to obtain process and thread identifiers.
namespace aqosa { class ProcessIdentifier
{
private:

    // Constructor is not defined - this is a utility class.
    ProcessIdentifier(void);

public:

    // Returns the name of the current process.
    static inline std::string currentProcessName(void)
    {
#ifdef AQ_TEST_UNIT
        if (m_fix)
        {
            return m_fixProcessName;
        }
#endif
        char *str;
        _get_pgmptr(&str);
        const char *s = str;
        for (size_t i = 0; str[i] != '\0'; ++i)
        {
            if (str[i] == '\\')
            {
                s = &str[i + 1];
            }
        }
        return std::string(s);
    }

    // Returns the 32-bit process identifier for the current process.
    static inline uint32_t currentProcessId(void)
    {
#ifdef AQ_TEST_UNIT
        if (m_fix)
        {
            return m_fixProcessId;
        }
#endif
        return (uint32_t)GetCurrentProcessId();
    }

    // Returns the 32-bit thread identifier for the current thread.
    static inline uint32_t currentThreadId(void)
    {
#ifdef AQ_TEST_UNIT
        if (m_fix)
        {
            return m_fixThreadId;
        }
#endif
        return (uint32_t)GetCurrentThreadId();
    }

    // Sets the process identifier to return fixed values.  If processName is NULL
    // this disables the fixed value return.
#ifdef AQ_TEST_UNIT
    static void fixProcessId(const char *processName, uint32_t processId, uint32_t threadId)
    {
        m_fix = processName != NULL;
        if (m_fix)
        {
            m_fixProcessName = processName;
            m_fixProcessId = processId;
            m_fixThreadId = threadId;
        }
    }

private:

    // Set to true to enable fixed values.
    static bool m_fix;

    // Set to the fixed process name.
    static std::string m_fixProcessName;

    // Set to the fixed process identifier.
    static uint32_t m_fixProcessId;

    // Set to the fixed thread identifier.
    static uint32_t m_fixThreadId;
#else
public:
    static void fixProcessId(const char *processName, uint32_t processId, uint32_t threadId) { }
#endif

};}




#endif
//=============================== End of File ==================================
