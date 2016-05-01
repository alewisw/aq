#ifndef TESTASSERT_H
#define TESTASSERT_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <string>
#include <sstream>



//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------

// The two main assertion macros - CHECK() does not fail the test immediatly,
// however it will fail at the end.  REQUIRE() causes an immediate test failure.
#define CHECK(__expr)                   TEST_ASSERT(false, __expr)
#define REQUIRE(__expr)                 TEST_ASSERT(true, __expr)

#define CHECK_EXCEPTION(__expr, __ex)   TEST_EXCEPTION(false, __expr, __ex)
#define REQUIRE_EXCEPTION(__expr, __ex) TEST_EXCEPTION(true, __expr, __ex)


// Asserts that an exception of type __exception is thrown when the expression
// __expr is executed.
#define TEST_EXCEPTION(__throwIfFalse, __expr, __exception)                     \
do                                                                              \
{                                                                               \
    bool __thrown = false;                                                      \
    try                                                                         \
    {                                                                           \
        __expr;                                                                 \
    }                                                                           \
    catch (const __exception&)                                                  \
    {                                                                           \
        __thrown = true;                                                        \
    }                                                                           \
    if (!__thrown)                                                              \
    {                                                                           \
        TEST_ASSERT_FAILED(EXPECTED_EXCEPTION, __throwIfFalse,                  \
          #__expr, #__exception);                                               \
    }                                                                           \
} while(0)


// Asserts that the passed expression '__expr' evaluates to 'true'.  If the value
// is false then:
//  - A TestAssert object is constructed and added to the current test 
//    execution.
//  - If '__throwIfFalse' is true then the TestAssert object is then thrown.
//  - 
#define TEST_ASSERT(__throwIfFalse, __expr)                                     \
do                                                                              \
{                                                                               \
    TestExecution::incrementAssertionCounter();                                 \
    TestAssert::Decomposer __decomposer;                                        \
    __decomposer >= __expr;                                                     \
    if (!__decomposer.outcome())                                                \
    {                                                                           \
        std::string exp;                                                        \
        try                                                                     \
        {                                                                       \
            exp = __decomposer.str();                                           \
        }                                                                       \
        catch (...)                                                             \
        {                                                                       \
        }                                                                       \
        TEST_ASSERT_FAILED(ASSERT, __throwIfFalse, #__expr, exp.c_str());       \
    }                                                                           \
} while(0)

// Used internally when an assertion fails.
#define TEST_ASSERT_FAILED(__type, __throwIfFailed, __exprStr, __exOrDecompStr) \
do                                                                              \
{                                                                               \
    TestAssert ast(__FILE__, __FUNCTION__, __LINE__, TestAssert::__type,        \
                   __exprStr, __exOrDecompStr);                                 \
    TestExecution::addAssertion(ast);                                           \
    if (__throwIfFailed && !TestExecution::isThrowing())                        \
    {                                                                           \
        TestExecution::markThrowing();                                          \
        throw ast;                                                              \
    }                                                                           \
} while(0)




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class TestExecution;



//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

// Encapsulates all the information regarding an assertion failure.
class TestAssert
{
public:

    // Forward declaration required for the Lhs class.
    class Decomposer;

    // Representing the LHS of an expression this is used to extract the RHS of 
    // the expression.
    template<typename T> class Lhs
    {
    public:
        // Constructs a new LHS object owned by 'owner' with the given initial
        // LHS value.
        Lhs(Decomposer& owner, T const &lhs) : m_owner(owner), m_lhs(lhs)
        {
            m_ss << lhs;
        }

        // Constructs a new LHS object owned by 'owner' with the given initial
        // LHS value.
        Lhs(const Lhs<T>& other) : m_owner(other.m_owner), m_lhs(other.m_lhs)
        {
            m_ss << other.m_ss.str();
        }

    private:
        // Holds the owner of this LHS.
        Decomposer& m_owner;

        // The LHS of the equation.
        T const &m_lhs;

        // Holds the decomposed string.
        std::ostringstream m_ss;

    public:
        // Decomposer for the '==' operator.
        Decomposer& operator==(T const& rhs)
        {
            m_ss << " == " << rhs;
            m_owner.store(m_ss);
            m_owner.m_outcome = m_lhs == rhs;
            return m_owner;
        }

        // Decomposer for the '!=' operator.
        Decomposer& operator!=(T const& rhs)
        {
            m_ss << " != " << rhs;
            m_owner.store(m_ss);
            m_owner.m_outcome = m_lhs != rhs;
            return m_owner;
        }

        // Decomposer for the '<' operator.
        Decomposer& operator<(T const& rhs)
        {
            m_ss << " < " << rhs;
            m_owner.store(m_ss);
            m_owner.m_outcome = m_lhs < rhs;
            return m_owner;
        }

        // Decomposer for the '>' operator.
        Decomposer& operator>(T const& rhs)
        {
            m_ss << " > " << rhs;
            m_owner.store(m_ss);
            m_owner.m_outcome = m_lhs > rhs;
            return m_owner;
        }

        // Decomposer for the '<=' operator.
        Decomposer& operator<=(T const& rhs)
        {
            m_ss << " <= " << rhs;
            m_owner.store(m_ss);
            m_owner.m_outcome = m_lhs <= rhs;
            return m_owner;
        }

        // Decomposer for the '>=' operator.
        Decomposer& operator>=(T const& rhs)
        {
            m_ss << " >= " << rhs;
            m_owner.store(m_ss);
            m_owner.m_outcome = m_lhs >= rhs;
            return m_owner;
        }

    };

    // Used to decompose an expression LHS and RHS.
    class Decomposer
    {
    public:

        // Constructs a new decomposer with a default outcome of false.
        Decomposer(void) : m_outcome(false) { }

    public:

        // Stores the outcome of the decomposition, called by the LHS object.
        void store(const std::ostringstream& ss)
        {
            m_str = ss.str();
        }

    private:

        // The outcome of the decomposition.
        std::string m_str;

    public:

        // The outcome recorded for this assertion.
        bool m_outcome;

        // Operator used to trigger the decomposition operation.
        template<typename T> Lhs<T> operator>=(T const& operand)
        {
            Lhs<T> lhs(*this, operand);
            std::ostringstream ss;
            ss << operand;
            m_outcome = ss.str() != "0";
            return lhs;
        }

        // Returns the decomposed string.
        const std::string &str(void) const { return m_str; }

        // Returns the outcome of the comparison operation.
        bool outcome(void) const { return m_outcome; }

    };

    // The types of assertion failures.
    enum Type
    {
        // A normal assertion failure.
        ASSERT,

        // This was an exception that was expected but did not occur.
        EXPECTED_EXCEPTION,

        // This was an exception that was unexpected.
        UNEXPECTED_EXCEPTION,
    };

    // Constructs a new test assertion failure at the specified location.
    //
    // 'expr':
    //   ASSERT -> The expression that failed to evaluate to 'TRUE'.
    //   EXPECTED_EXCEPTION -> The expression that failed to throw the exception.
    //   UNEXPECTED_EXCEPTION -> The exception text, if it could be found.
    //
    // 'decomp':
    //   ASSERT -> The decomposed expression, if available.
    //   EXPECTED_EXCEPTION -> The name of the exception.
    //   UNEXPECTED_EXCEPTION -> The name of the exception, if known.
    TestAssert(const char *file, const char *func, int line,
        Type type, const std::string& expr, const std::string& decomp);

    // Creates an exact copy of this test assertion failure.
    TestAssert(const TestAssert& other);

    // Assigns the value of this test assertion failure to exactly match another.
    TestAssert& operator=(const TestAssert& other);

    // Destroys this assertion failure report.
    ~TestAssert(void);

private:

    // The file where the assertion occurred.
    const char *m_file;

    // The function in which the assertion occurred.
    const char *m_func;

    // The line on which the assertion occurred.
    int m_line;

    // The type of exception.
    Type m_type;

    // The expression that failed.
    std::string m_expr;

    // The decomposed expression or Exception type that was thrown.
    std::string m_decomp;

    // The execution, attached to this assertion.
    const TestExecution *m_exec;

    // An empty string.
    static std::string m_emptyString;

public:

    // The file where the assertion occurred.
    const char *file(void) const { return m_file; }

    // The function in which the assertion occurred.
    const char *function(void) const { return m_func; }

    // The line on which the assertion occurred.
    int line(void) const { return m_line; }

    // The type of assertion failure.
    Type type(void) const { return m_type; }

    // The expression related to the failure.
    const std::string& expr(void) const 
    { 
        return type() == UNEXPECTED_EXCEPTION ? m_emptyString : m_expr; 
    }

    // The decomposition of the expression related to the failure.
    const std::string& exprDecomp(void) const 
    {
        return type() == ASSERT ? m_decomp : m_emptyString; 
    }

    // The name of the exception, if any, related to this assetion failure.
    const std::string& exceptionName(void) const 
    { 
        return type() == ASSERT ? m_emptyString : m_decomp; 
    }

    // The name of the exception, if any, related to this assetion failure.
    const std::string& exceptionMessage(void) const
    {
        return type() == UNEXPECTED_EXCEPTION ? m_expr : m_emptyString;
    }

    // The execution attached to this exception.
    const TestExecution& execution(void) const { return *m_exec; }

    // Attaches an execution to this exception.
    void setExecution(TestExecution *exec) { m_exec = exec; }

};




#endif
//=============================== End of File ==================================
