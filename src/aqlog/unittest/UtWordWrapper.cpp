//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "Main.h"

#include "AQLogStringBuilder.h"
#include "WordWrapper.h"




//------------------------------------------------------------------------------
// Private Macros
//------------------------------------------------------------------------------

// The number of newline combinations to test.
#define NEWLINE_COMBO_COUNT             (sizeof(NewlineCombo) / sizeof(NewlineCombo_t))

//#define DUMP_ENABLED

#define SPACES_10                       "          "
#define SPACES_100                      SPACES_10 SPACES_10 SPACES_10 SPACES_10 SPACES_10 SPACES_10 SPACES_10 SPACES_10 SPACES_10 SPACES_10
#define SPACES_1000                     SPACES_100 SPACES_100 SPACES_100 SPACES_100 SPACES_100 SPACES_100 SPACES_100 SPACES_100 SPACES_100 SPACES_100



//------------------------------------------------------------------------------
// Private Type Definitions
//------------------------------------------------------------------------------

// Defines a combination of newline input string, newline, prefix spaces count 
// and newline output string.
struct NewlineCombo_t
{
    // The input newline to use.
    const char *inStr;

    // The newline type to pass to the word wrapper.
    WordWrapper::Newline nl;

    // The number of prefix spaces to use.
    uint32_t prefixSpaces;

    // The expected output newline string.
    const char *outStr;

};




//------------------------------------------------------------------------------
// Private Function and Class Declarations
//------------------------------------------------------------------------------

// Takes the string 'in' and translates the newlines based on combo, returning
// a string allocated with malloc().  The target newline is inStr.
static char *TranslateNLInput(const NewlineCombo_t& combo, const char *in);

// Takes the string 'out' and translates the newlines based on combo, returning
// a string.  The target newline is outStr.
static string TranslateNLOutput(const NewlineCombo_t& combo, const char *out);

// Dumps a combo configuration to stdout.
static void DumpCombo(const NewlineCombo_t& combo);

// Dumps the passed string to the console and returns that string.
static string DumpString(string s, bool appendEndl = true);




//------------------------------------------------------------------------------
// Variable Declarations
//------------------------------------------------------------------------------

static const NewlineCombo_t NewlineCombo[] =
{
    { "\n", WordWrapper::Newline::NEWLINE_LF, 0, "\n" },
    { "\r\n", WordWrapper::Newline::NEWLINE_LF, 0, "\n" },
    { "\n\r", WordWrapper::Newline::NEWLINE_LF, 0, "\n" },

    { "\n", WordWrapper::Newline::NEWLINE_CRLF, 0, "\r\n" },
    { "\r\n", WordWrapper::Newline::NEWLINE_CRLF, 0, "\r\n" },
    { "\n\r", WordWrapper::Newline::NEWLINE_CRLF, 0, "\r\n" },

    { "\n", WordWrapper::Newline::NEWLINE_LF, 1, "\n " },
    { "\r\n", WordWrapper::Newline::NEWLINE_LF, 5, "\n     " },
    { "\n\r", WordWrapper::Newline::NEWLINE_LF, 10, "\n          " },

    { "\n", WordWrapper::Newline::NEWLINE_CRLF, 1000, "\r\n" SPACES_1000 },
    { "\r\n", WordWrapper::Newline::NEWLINE_CRLF, 1010, "\r\n" SPACES_1000 SPACES_10 },
    { "\n\r", WordWrapper::Newline::NEWLINE_CRLF, 1110, "\r\n" SPACES_1000 SPACES_100 SPACES_10 },
};



//------------------------------------------------------------------------------
// Function and Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TEST_SUITE(UtWordWrapper);

//------------------------------------------------------------------------------
TEST(given_String_when_WordWrapUnlimitedCols_then_Unchanged)
{
    AQLogStringBuilder src;
    AQLogStringBuilder dst;
    src.appendPointer("At vero eos et accusamus et iusto odio dignissimos ducimus");
    WordWrapper::appendWordWrapped(src, dst, 1000, 0);
    REQUIRE(DumpString(dst.toString()) ==
        "At vero eos et accusamus et iusto odio dignissimos ducimus");
}

//------------------------------------------------------------------------------
TEST(given_StringEndsWithSpace_when_WordWrapUnlimitedCols_then_EndSpaceTrimmed)
{
    AQLogStringBuilder src;
    AQLogStringBuilder dst;
    src.appendPointer("At vero eos et accusamus et iusto odio dignissimos ducimus \t ");
    WordWrapper::appendWordWrapped(src, dst, 1000, 0);
    REQUIRE(DumpString(dst.toString()) ==
        "At vero eos et accusamus et iusto odio dignissimos ducimus");
}
//------------------------------------------------------------------------------
TEST(given_StringEndsWithNL_when_WordWrapUnlimitedCols_then_OneLine)
{
    for (size_t i = 0; i < NEWLINE_COMBO_COUNT; ++i)
    {
        DumpCombo(NewlineCombo[i]);

        AQLogStringBuilder src;
        AQLogStringBuilder dst;
        src.appendFree(TranslateNLInput(NewlineCombo[i], 
            "At vero eos et accusamus et iusto odio dignissimos ducimus\n"));
        WordWrapper::appendWordWrapped(src, dst, 1000, NewlineCombo[i].prefixSpaces, NewlineCombo[i].nl);
        DumpString(dst.toString());
        REQUIRE(dst.toString() == TranslateNLOutput(NewlineCombo[i],
            "At vero eos et accusamus et iusto odio dignissimos ducimus"));
    }
}

//------------------------------------------------------------------------------
TEST(given_StringEndsWithNLThenTab_when_WordWrapUnlimitedCols_then_TwoLines)
{
    for (size_t i = 0; i < NEWLINE_COMBO_COUNT; ++i)
    {
        DumpCombo(NewlineCombo[i]);

        AQLogStringBuilder src;
        AQLogStringBuilder dst;
        src.appendFree(TranslateNLInput(NewlineCombo[i],
            "At vero eos et accusamus et iusto odio dignissimos ducimus\n\t"));
        WordWrapper::appendWordWrapped(src, dst, 1000, NewlineCombo[i].prefixSpaces, NewlineCombo[i].nl);
        REQUIRE(DumpString(dst.toString()) == TranslateNLOutput(NewlineCombo[i],
            "At vero eos et accusamus et iusto odio dignissimos ducimus\n"
            ""));
    }
}

//------------------------------------------------------------------------------
TEST(given_StringEndsWithNLThenSpace_when_WordWrapUnlimitedCols_then_TwoLines)
{
    for (size_t i = 0; i < NEWLINE_COMBO_COUNT; ++i)
    {
        DumpCombo(NewlineCombo[i]);

        AQLogStringBuilder src;
        AQLogStringBuilder dst;
        src.appendFree(TranslateNLInput(NewlineCombo[i],
            "At vero eos et accusamus et iusto odio dignissimos ducimus\n "));
        WordWrapper::appendWordWrapped(src, dst, 1000, NewlineCombo[i].prefixSpaces, NewlineCombo[i].nl);
        REQUIRE(DumpString(dst.toString()) == TranslateNLOutput(NewlineCombo[i],
            "At vero eos et accusamus et iusto odio dignissimos ducimus\n"
            ""));
    }
}

//------------------------------------------------------------------------------
TEST(given_StringEndsWithNLThenMultiWhitespace_when_WordWrapUnlimitedCols_then_TwoLines)
{
    for (size_t i = 0; i < NEWLINE_COMBO_COUNT; ++i)
    {
        DumpCombo(NewlineCombo[i]);

        AQLogStringBuilder src;
        AQLogStringBuilder dst;
        src.appendFree(TranslateNLInput(NewlineCombo[i],
            "At vero eos et accusamus et iusto odio dignissimos ducimus\n \t\t "));
        WordWrapper::appendWordWrapped(src, dst, 1000, NewlineCombo[i].prefixSpaces, NewlineCombo[i].nl);
        REQUIRE(DumpString(dst.toString()) == TranslateNLOutput(NewlineCombo[i],
            "At vero eos et accusamus et iusto odio dignissimos ducimus\n"
            ""));
    }
}

//------------------------------------------------------------------------------
TEST(given_StringContainsWhitespaceBeforeNL_when_WordWrapUnlimitedCols_then_OneLinePerNL)
{
    for (size_t i = 0; i < NEWLINE_COMBO_COUNT; ++i)
    {
        DumpCombo(NewlineCombo[i]);

        AQLogStringBuilder src;
        AQLogStringBuilder dst;
        src.appendFree(TranslateNLInput(NewlineCombo[i],
            "At vero eos et accusamus\t\n"
            "et iusto \n"
            "odio dignissimos ducimus \t    \n"));
        WordWrapper::appendWordWrapped(src, dst, 1000, NewlineCombo[i].prefixSpaces, NewlineCombo[i].nl);
        REQUIRE(DumpString(dst.toString()) == TranslateNLOutput(NewlineCombo[i],
            "At vero eos et accusamus\n"
            "et iusto\n"
            "odio dignissimos ducimus"));
    }
}

//------------------------------------------------------------------------------
TEST(given_StringContainsWhitespaceAfterNL_when_WordWrapUnlimitedCols_then_OneLinePerNL)
{
    for (size_t i = 0; i < NEWLINE_COMBO_COUNT; ++i)
    {
        DumpCombo(NewlineCombo[i]);

        AQLogStringBuilder src;
        AQLogStringBuilder dst;
        src.appendFree(TranslateNLInput(NewlineCombo[i],
            "At vero eos et accusamus\n"
            "\tet iusto \n"
            "  odio dignissimos ducimus\n "));
        WordWrapper::appendWordWrapped(src, dst, 1000, NewlineCombo[i].prefixSpaces, NewlineCombo[i].nl);
        REQUIRE(DumpString(dst.toString()) == TranslateNLOutput(NewlineCombo[i],
            "At vero eos et accusamus\n"
            "\tet iusto\n"
            "  odio dignissimos ducimus\n"
            ""));
    }
}

//------------------------------------------------------------------------------
TEST(given_StringContainsNoWhitespace_when_WordWrap10Cols_then_OneLine)
{
    AQLogStringBuilder src;
    AQLogStringBuilder dst;
    src.appendPointer("At_vero_eos_et_accusamus_et_iusto_odio_dignissimos_ducimus");
    WordWrapper::appendWordWrapped(src, dst, 1000, 10);
    REQUIRE(DumpString(dst.toString()) ==
        "At_vero_eos_et_accusamus_et_iusto_odio_dignissimos_ducimus");
}

//------------------------------------------------------------------------------
TEST(given_StringContainsWhitespaceAfter10Cols_when_WordWrap10Cols_then_WrapsAtSpaces)
{
    for (size_t i = 0; i < NEWLINE_COMBO_COUNT; ++i)
    {
        DumpCombo(NewlineCombo[i]);

        AQLogStringBuilder src;
        AQLogStringBuilder dst;
        src.appendFree(TranslateNLInput(NewlineCombo[i],
            "At_vero_eos_et \t     accusamus_et iusto_odio_dignissimos\tducimus  "));
        WordWrapper::appendWordWrapped(src, dst, 10, NewlineCombo[i].prefixSpaces, NewlineCombo[i].nl);
        REQUIRE(DumpString(dst.toString()) == TranslateNLOutput(NewlineCombo[i],
            "At_vero_eos_et\n"
            "accusamus_et\n"
            "iusto_odio_dignissimos\n"
            "ducimus"));
    }
}

//------------------------------------------------------------------------------
TEST(given_StringContainsWhitespaceBefore10Cols_when_WordWrap10Cols_then_WrapsBefore10Cols)
{
    for (size_t i = 0; i < NEWLINE_COMBO_COUNT; ++i)
    {
        DumpCombo(NewlineCombo[i]);

        AQLogStringBuilder src;
        AQLogStringBuilder dst;
        src.appendFree(TranslateNLInput(NewlineCombo[i],
            "At vero eos et\taccusamus    et iusto\t\t\todio  \t  \tdignissimos \t   \tducimus   \t"));
        WordWrapper::appendWordWrapped(src, dst, 10, NewlineCombo[i].prefixSpaces, NewlineCombo[i].nl);
        REQUIRE(DumpString(dst.toString()) == TranslateNLOutput(NewlineCombo[i],
            "At vero\n"
            "eos et\n"
            "accusamus\n"
            "et iusto\n"
            "odio\n"
            "dignissimos\n"
            "ducimus"));
    }
}
//------------------------------------------------------------------------------
static char *TranslateNLInput(const NewlineCombo_t& combo, const char *in)
{
    ostringstream ss;

    for (size_t i = 0; in[i] != '\0'; ++i)
    {
        if (in[i] == '\n')
        {
            ss << combo.inStr;
        }
        else
        {
            ss << in[i];
        }
    }

    string s = ss.str();
    char *str = (char *)malloc(s.size() + 1);
    memcpy(str, s.c_str(), s.size() + 1);
    return str;
}

//------------------------------------------------------------------------------
static string TranslateNLOutput(const NewlineCombo_t& combo, const char *out)
{
    ostringstream ss;

    for (size_t i = 0; out[i] != '\0'; ++i)
    {
        if (out[i] == '\n')
        {
            ss << combo.outStr;
        }
        else
        {
            ss << out[i];
        }
    }

    return ss.str();
}

//------------------------------------------------------------------------------
static void DumpCombo(const NewlineCombo_t& combo)
{
#ifdef DUMP_ENABLED
    cout << "Test Combo: prefix = " << combo.prefixSpaces;
    if (combo.prefixSpaces == 1)
    {
        cout << " space";
    }
    else
    {
        cout << " spaces";
    }
    cout << ", input newline = \"";
    DumpString(string(combo.inStr), false);
    cout << "\", output newline = \"";
    if (combo.nl == WordWrapper::NEWLINE_CRLF)
    {
        cout << "\\r\\n";
    }
    else
    {
        cout << "\\n";
    }
    cout << "\"" << endl;
#endif
}

//------------------------------------------------------------------------------
static string DumpString(string s, bool appendEndl)
{
#ifdef DUMP_ENABLED
    for (size_t i = 0; i < s.size(); ++i)
    {
        switch (s[i])
        {
        case '\r':
            cout << "\\r";
            break;
        case '\n':
            cout << "\\n";
            if (appendEndl)
            {
                cout << endl;
            }
            break;
        case '\t':
            cout << "\\t";
            break;
        case ' ':
            cout << "\\s";
            break;
        default:
            if (s[i] < 32)
            {
                cout << "\\{" << (int)s[i] << "}";
            }
            else
            {
                cout << s[i];
            }
            break;
        }
    }
    if (appendEndl)
    {
        cout << endl;
    }
#endif
    return s;
}




//=============================== End of File ==================================
