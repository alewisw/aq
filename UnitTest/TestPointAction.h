#ifndef TESTPOINTACTION_H
#define TESTPOINTACTION_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================
#ifdef AQ_TEST_POINT

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "AQReader.h"
#include "AQWriter.h"
#include "AQWriterItem.h"
#include "AQSnapshot.h"

#include "TestPointNotifier.h"

#include <stdexcept>
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

// Base class for the TestPointAction - used to perform the necessary callback.
class TestPointActionCaller
{
protected:

    // Constructor and destructor - these do nothing.
    TestPointActionCaller(void) { }
    virtual ~TestPointActionCaller(void) { }

public:

    // Callback function.
    virtual void testPointInstanceCallback(void) = 0;

private:

    // The callback used for this test point.
    static void testPointCallback(AQ *queue, void *context);

public:

    // Attaches this test point action directly to its queue.
    void attach(AQ& queue, int tp)
    {
#ifdef AQ_TEST_POINT
        attach(queue.testPointNotifier(), tp);
#endif
    }

    // Attaches this test point action directly to its queue.
    void attach(aq::TestPointNotifier *tpn, int tp)
    {
        tpn->registerTestPoint(tp, testPointCallback, NULL, this);
    }

};

// Defines an action to take when a test point is reached.
template<class Q> class TestPointAction : public TestPointActionCaller
{

public:
    // Construct a new test point action to execute on a particular queue.
    TestPointAction(Q& queue)
        : m_queue(queue)
    {

    }

    // Destroy this test point action.
    virtual ~TestPointAction(void)
    {
    }

private:

    // Defines a chaining of actions.
    struct Chain
    {
        // The queue where the action is to be attached.
        aq::TestPointNotifier *tpn;

        // The test point.
        int tp;

        // The action.
        TestPointActionCaller *action;
    };

public:

    // Chains another action to be configured before this action is executed.
    void chainBefore(AQ& queue, int tp, TestPointAction& preAction)
    {
        Chain c;

#ifdef AQ_TEST_POINT
        c.tpn = queue.testPointNotifier();
#endif
        c.tp = tp;
        c.action = &preAction;
        m_chainBefore.push_back(c);
    }

    // Chains another action to be configured after this action is executed.
    void chainAfter(AQ& queue, int tp, TestPointAction& postAction)
    {
        Chain c;

        c.tpn = queue.testPointNotifier();
        c.tp = tp;
        c.action = &postAction;
        m_chainAfter.push_back(c);
    }

    // Runs the passed action after this action.
    void runAfter(TestPointActionCaller &after)
    {
        m_runAfter.push_back(&after);
    }

private:

    // The queue.
    Q& m_queue;

    // The test point.
    int m_tp;

    // The list of actions to chain before and after this action.
    std::vector<Chain> m_chainBefore;
    std::vector<Chain> m_chainAfter;
    std::vector<TestPointActionCaller *> m_runAfter;

protected:

    // The instance callback called from testPointCallback().
    virtual void testPointInstanceCallback(void)
    {
        for (size_t i = 0; i < m_chainBefore.size(); ++i)
        {
            m_chainBefore[i].action->attach(m_chainBefore[i].tpn, m_chainBefore[i].tp);
        }
        testPoint(m_queue);
        for (size_t i = 0; i < m_chainAfter.size(); ++i)
        {
            m_chainAfter[i].action->attach(m_chainAfter[i].tpn, m_chainAfter[i].tp);
        }
        for (size_t i = 0; i < m_runAfter.size(); ++i)
        {
            m_runAfter[i]->testPointInstanceCallback();
        }
    }

    // The function that performs the actual work on the test point.
    virtual void testPoint(Q& queue) = 0;

};

// This action performs a claim against a particuar queue.
class ClaimAction : public TestPointAction<AQWriter>
{
public:
    // Perform a claim on 'queue' of size 'size'.
    ClaimAction(AQWriter& queue, size_t size)
        : TestPointAction<AQWriter>(queue)
        , m_size(size)
        , m_result(false) { }
    virtual ~ClaimAction(void) { }

private:

    // The size of the claim action.
    size_t m_size;

    // The result of the operation.
    bool m_result;

    // The item that was claimed.
    AQWriterItem m_item;

public:

    // The function that performs the actual work on the test point.
    virtual void testPoint(AQWriter& queue)
    {
        m_result = queue.claim(m_item, m_size);
    }

    // The results of the claim operation.
    bool result(void) const { return m_result; }
    AQWriterItem& item(void) { return m_item; }

};

// This action performs a series of aq.enqueue actions against a particuar queue.
class EnqueueAction : public TestPointAction<AQWriter>
{
public:
    // Perform a series of claim/commit actions on 'queue'.
    EnqueueAction(AQWriter& queue, int count, size_t size = 1)
        : TestPointAction<AQWriter>(queue)
        , m_count(count)
        , m_size(size) { }
    virtual ~EnqueueAction(void) { }

private:

    // The number of items to aq.enqueue.
    int m_count;

    // The size of each item.
    size_t m_size;

public:

    // The function that performs the actual work on the test point.
    virtual void testPoint(AQWriter& queue)
    {
        AQWriterItem rec;

        for (int i = 0; i < m_count; ++i)
        {
            queue.claim(rec, m_size);
            queue.commit(rec);
        }
    }

};

// This action performs a claim against a particuar queue.
class CommitAction : public TestPointAction<AQWriter>
{
public:
    // Perform a claim on 'queue' of size 'size'.
    CommitAction(AQWriter& queue, AQWriterItem& item)
        : TestPointAction<AQWriter>(queue)
        , m_item(item)
        , m_result(false) { }
    virtual ~CommitAction(void) { }

private:

    // The result of the operation.
    bool m_result;

    // The item that was claimed.
    AQWriterItem& m_item;

public:

    // The function that performs the actual work on the test point.
    virtual void testPoint(AQWriter& queue)
    {
        m_result = queue.commit(m_item);
    }

    // The results of the claim operation.
    bool result(void) const { return m_result; }

};

// This action performs a Retrieve against a particuar queue.
class RetrieveAction : public TestPointAction<AQReader>
{
public:
    // Perform a Retrieve on 'queue' of size 'size'.
    RetrieveAction(AQReader& queue)
        : TestPointAction<AQReader>(queue)
        , m_result(false) { }
    virtual ~RetrieveAction(void) { }

private:

    // The result of the operation.
    bool m_result;

    // The item that was Retrieveed.
    AQItem m_item;

public:

    // The function that performs the actual work on the test point.
    virtual void testPoint(AQReader& queue)
    {
        m_result = queue.retrieve(m_item);
    }

    // The results of the Retrieve operation.
    bool result(void) const { return m_result; }
    AQItem& item(void) { return m_item; }

};

// This action performs a release() against a particuar queue.
class ReleaseAction : public TestPointAction<AQReader>
{
public:
    // Perform a Retrieve on 'queue' of size 'size'.
    ReleaseAction(AQReader& queue, AQItem& item)
        : TestPointAction<AQReader>(queue)
        , m_item(item) { }
    virtual ~ReleaseAction(void) { }

private:

    // The item that was Retrieveed.
    AQItem& m_item;

public:

    // The function that performs the actual work on the test point.
    virtual void testPoint(AQReader& queue)
    {
        queue.release(m_item);
    }

};

// This action performs a simulated crash at a particular test point.
class ThrowAction : public TestPointAction<AQ>
{
public:
    // Perform a claim on 'queue' of size 'size'.
    ThrowAction(AQ& queue)
        : TestPointAction<AQ>(queue) { }
    virtual ~ThrowAction(void) { }

public:

    // The function that performs the actual work on the test point.
    virtual void testPoint(AQ& queue)
    {
        throw std::domain_error("throw!");
    }

};

// This action performs one of the snapshot stages at a particular test point.
class SnapshotAction : public TestPointAction<AQ>
{
public:
    // Perform a claim on 'queue' of size 'size'.
    SnapshotAction(AQ &queue, AQSnapshot& snap, int step0, int step1 = -1, int step2 = -1, int step3 = -1)
        : TestPointAction<AQ>(queue)
        , m_snap(snap)
    {
        if (step0 >= 1 && step0 <= 5) m_stages.push_back(step0);
        if (step1 >= 1 && step1 <= 5) m_stages.push_back(step1);
        if (step2 >= 1 && step2 <= 5) m_stages.push_back(step2);
        if (step3 >= 1 && step3 <= 5) m_stages.push_back(step3);
    }
    virtual ~SnapshotAction(void) { }

public:

    // The snapshot.
    AQSnapshot &m_snap;

    // The snapshot stages to execute.
    std::vector<int> m_stages;

    // The function that performs the actual work on the test point.
    virtual void testPoint(AQ& queue)
    {
        for (size_t i = 0; i < m_stages.size(); ++i)
        {
            switch (m_stages[i])
            {
            case 1: m_snap.snap1InitialHead(queue); break;
            case 2: m_snap.snap2InitialCtrlq();     break;
            case 3: m_snap.snap3PageMemory();       break;
            case 4: m_snap.snap4FinalHead();        break;
            }
        }
    }

};




#endif
#endif
//=============================== End of File ==================================
